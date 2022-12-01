#include "dependencies/std_include.hpp"
#include "instant_message.hpp"

namespace events::instant_message
{
	bool log_messages = true;

	namespace
	{
		std::string get_sender_string(const std::uint64_t sender_id)
		{
			const auto sender_name{ steam::get_friend_persona_name(sender_id) };

			if (!sender_name.empty())
			{
				return utils::string::va("'%s' (%llu)", sender_name.data(), sender_id);
			}

			return utils::string::va("(%llu)", sender_id);
		}
		
		using callback = std::function<bool(game::msg_t&, const std::uint64_t)>;
		
		std::unordered_map<std::uint8_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uint8_t, callback> callbacks{};
			return callbacks;
		}
		
		void on_message(const std::uint8_t type, const callback& callback)
		{
			get_callbacks()[type] = callback;
		}
		
		bool handle_message(const std::uint64_t sender_id, const std::uint8_t type, game::msg_t& msg)
		{
			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(type);

			if (handler == callbacks.end())
			{
				return false;
			}

			return handler->second(msg, sender_id);
		}
	}

	bool __fastcall dw_instant_dispatch_message_stub(std::uint64_t sender_id, ControllerIndex_t controller_index, std::uint8_t* message, std::uint32_t message_size)
	{
		if (message_size >= 0x1000)
		{
			PRINT_LOG("Received instant message of invalid size [%u] from %s", message_size, get_sender_string(sender_id).data());
			return false;
		}

		game::msg_t msg{};
		game::MSG_InitReadOnly(&msg, message, message_size);
		game::MSG_BeginReading(&msg);

		auto type{ 0ui8 };

		if (msg.read<uint8_t>() == '1')
		{
			type = msg.read<uint8_t>();
		}

		if (log_messages)
		{
			PRINT_LOG("Received instant message '%c' of size [%u] from %s", type, message_size, get_sender_string(sender_id).data());
		}

		return handle_message(sender_id, type, msg);
	}

	bool send_info_request(const std::vector<std::uint64_t>& recipients, uint32_t nonce)
	{
		if (game::Live_IsUserSignedInToDemonware(0))
		{
			char buffer[0x80] = { 0 };
			game::msg_t msg{};

			msg.init_lobby(buffer, game::MESSAGE_TYPE_INFO_REQUEST);
			game::LobbyMsgRW_PackageUInt(&msg, "nonce", &nonce);

			return game::send_instant_message(recipients, 'h', msg);
		}

		return false;
	}
	
	void initialize()
	{
		utils::hook::jump(game::base_address + 0x1439600, instant_message::dw_instant_dispatch_message_stub);
		
		instant_message::on_message('f', [=](auto& msg, const auto& sender_id)
		{
			game::JoinSessionMessage message{};

			if (msg.cursize - msg.readcount == sizeof message)
			{
				game::MSG_ReadData(&msg, &message, sizeof message);

				if (message.mType == game::JOIN_REQUEST)
				{
					PRINT_MESSAGE("Instant Message", "Crash attempt caught from %s", get_sender_string(sender_id).data());
				}
				else if (message.mType == game::JOIN_REPLY)
				{
					PRINT_MESSAGE("Instant Message", "Popup attempt caught from %s", get_sender_string(sender_id).data());
				}
			}
			else
			{
				PRINT_MESSAGE("Instant Message", "Popup attempt caught from %s", get_sender_string(sender_id).data());
			}

			return false;
		});
		
		instant_message::on_message('h', [=](auto& msg, const auto& sender_id)
		{
			const auto length{ std::min(msg.cursize - msg.readcount, 2048) };

			if (length < 0)
				return false;

			char buffer[2048] = { 0 };
			game::MSG_ReadData(&msg, buffer, length);

			if (msg.overflowed)
				return false;

			if (!game::LobbyMsgRW_PrepReadMsg(&msg, buffer, length))
				return false;

			if (lobby_msg::log_messages)
			{
				PRINT_LOG("Received lobby message <%s> from %s", game::LobbyTypes_GetMsgTypeName(msg.type), get_sender_string(sender_id).data());
			}

			if (msg.type == game::MESSAGE_TYPE_INFO_REQUEST)
			{
				uint32_t nonce{ 0 };
				
				if (!game::LobbyMsgRW_PackageUInt(&msg, "nonce", &nonce))
					return false;

				if (!game::s_infoProbe->active
				 && game::s_infoProbe->nonce != nonce
					&& nonce != friends::NONCE)
				{
					PRINT_MESSAGE("Instant Message", "Received a info request from %s", get_sender_string(sender_id).data());

					if (events::prevent_join)
						return false;
				}

				return game::LobbyJoinSource_IMInfoRequest(0, sender_id, nonce);
			}
			else if (msg.type == game::MESSAGE_TYPE_INFO_RESPONSE)
			{
				auto msg_backup{ msg };
				game::Msg_InfoResponse info_response{};

				if (!game::MSG_InfoResponse(&info_response, &msg))
					return false;

				if (info_response.nonce != friends::NONCE)
				{
					return game::LobbyJoinSource_IMInfoResponse(0, sender_id, &msg_backup);
				}

				std::vector<uint64_t> sender_xuids{ sender_id };

				for (size_t i = 0; i < std::size(info_response.lobby); ++i)
				{
					const auto lobby = info_response.lobby[i];
					if (lobby.isValid)
					{
						sender_xuids.push_back(lobby.hostXuid);
					}
				}

				for (const auto& id : sender_xuids)
				{
					if (const auto f = friends::get(id); f)
					{
						f->last_online = std::time(nullptr); 
						f->response.info_response = info_response;
					}
				}

				friends::write();

				return false;
			}

			return true;
		});
	}
}