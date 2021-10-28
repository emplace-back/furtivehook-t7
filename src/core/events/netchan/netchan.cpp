#include "dependencies/std_include.hpp"
#include "netchan.hpp"

namespace events::netchan
{
	std::unordered_map<game::MsgType, callback> messages;
	bool log_messages = false;
	
	void on_message(const game::MsgType& msg_type, const callback& callback)
	{
		if (messages.find(msg_type) == messages.end())
		{
			messages[msg_type] = callback;
		}
	}

	void log_netchan_messages(const game::netadr_t* from, const game::LobbyMsg* lobby_msg, const std::uint64_t sender_id)
	{
		if (!log_messages)
		{
			return logger::print_log("%s", game::get_netchan_message(*from, *lobby_msg, sender_id).data());
		}

		PRINT_LOG("%s", game::get_netchan_message(*from, *lobby_msg, sender_id).data());
	}
	
	bool handle_message(game::netadr_t* from, game::msg_t* msg, std::uint64_t sender_id)
	{
		game::LobbyMsg lobby_msg{};

		if (game::LobbyMsgRW_PrepReadMsg(&lobby_msg, msg))
		{
			if (const auto message_func = messages.find(lobby_msg.msgType); message_func != messages.end())
			{
				return message_func->second(*from, lobby_msg, sender_id);
			}

			log_netchan_messages(from, &lobby_msg, sender_id);
		}
		
		return false;
	}

	size_t handle_packet_internal()
	{
		static auto stub = utils::hook::assemble([](auto& a)
		{
			const auto return_original = a.newLabel();

			a.pushad64();

			a.mov(r8, rdi); // sender_id
			a.mov(rdx, rbx); // msg
			a.mov(rcx, rbp); // netadr

			a.call_aligned(netchan::handle_message);

			a.test(al, al);
			a.jz(return_original);

			a.popad64();
			a.jmp(game::base_address + 0x1EF71BD);

			a.bind(return_original);
			a.popad64();
			a.jmp(game::base_address + 0x1EF706B);
		});

		return reinterpret_cast<size_t>(stub);
	}

	void initialize()
	{
		exception::hbp::register_exception(game::base_address + 0x1EF7068, [](const auto& ex)
		{
			ex->ContextRecord->Rdx = ex->ContextRecord->Rbx;
			ex->ContextRecord->Rip = netchan::handle_packet_internal();
			return EXCEPTION_CONTINUE_EXECUTION;
		});
	}
}