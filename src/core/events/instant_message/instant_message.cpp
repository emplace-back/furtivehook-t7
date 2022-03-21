#include "dependencies/std_include.hpp"
#include "instant_message.hpp"

namespace events::instant_message
{
	bool log_messages = true, prevent_join = false;

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

	bool __fastcall dispatch_message(std::uint64_t sender_id, ControllerIndex_t controller_index, std::uint8_t* message, std::uint32_t message_size)
	{
		if (message_size >= 0x1000)
		{
			PRINT_LOG("Received instant message of invalid size [%u] from %s", message_size, get_sender_string(sender_id).data());
			return false;
		}

		game::msg_t msg{};
		game::MSG_InitReadOnly(&msg, message, message_size);
		game::MSG_BeginReading(&msg);

		auto type = 0ui8;

		if (game::MSG_ReadByte(&msg) == '1')
		{
			type = game::MSG_ReadByte(&msg);
		}

		if (log_messages)
		{
			PRINT_LOG("Received instant message '%c' of size [%u] from %s", type, message_size, get_sender_string(sender_id).data());
		}

		return handle_message(sender_id, type, msg);
	}

	bool send_info_request(const std::vector<std::uint64_t>& recipients)
	{
		if (game::Live_IsDemonwareFetchingDone(0))
		{
			char buffer[0x400] = { 0 };
			game::msg_t msg{};

			game::LobbyMsgRW_PrepWriteMsg(&msg, buffer, sizeof buffer, game::MESSAGE_TYPE_INFO_REQUEST);
			game::LobbyMsgRW_PackageUInt(&msg, "nonce", &friends::NONCE);

			return game::send_instant_message(recipients, 'h', msg);
		}

		return false;
	}
	
	void initialize()
	{
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

			if (!game::LobbyMsgRW_PrepReadData(&msg, buffer, length))
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

				if (!game::s_infoProbe.active
					&& game::s_infoProbe.nonce != nonce
					&& nonce != friends::NONCE)
				{
					PRINT_MESSAGE("Instant Message", "Received a info request from %s", get_sender_string(sender_id).data());

					if (instant_message::prevent_join)
						return false;
				}

				return game::LobbyJoinSource_IMInfoRequest(0, sender_id, nonce);
			}
			else if (msg.type == game::MESSAGE_TYPE_INFO_RESPONSE)
			{
				auto msg_backup{ msg };
				game::Msg_InfoResponse response{};

				if (!game::MSG_InfoResponse(&response, &msg))
					return false;
				
				if (response.nonce == friends::NONCE)
				{
					friends::add_friend_response(response, sender_id);
					return false;
				}

				if (game::s_infoProbe.active && game::s_infoProbe.nonce != response.nonce)
				{
					PRINT_MESSAGE("Instant Message", "Received info response from %s", get_sender_string(sender_id).data());
				}
				
				return game::LobbyJoinSource_IMInfoResponse(0, sender_id, &msg_backup);
			}

			return true;
		});
	}
}