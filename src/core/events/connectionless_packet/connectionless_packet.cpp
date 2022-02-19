#include "dependencies/std_include.hpp"
#include "connectionless_packet.hpp"

namespace events::connectionless_packet
{
	bool log_commands = true;

	namespace
	{
		std::string get_sender_string(const game::netadr_t& from)
		{
			if (const auto client_num = game::find_target_from_addr(from); client_num >= 0)
			{
				const auto target_client = game::session->clients[client_num].activeClient;

				return utils::string::va("'%s' (%llu) %s",
					target_client->fixedClientInfo.gamertag,
					target_client->fixedClientInfo.xuid,
					utils::string::adr_to_string(&from).data());
			}

			return utils::string::va("%s", utils::string::adr_to_string(&from).data());
		}

		std::unordered_map<std::string, callback>& get_callbacks()
		{
			static std::unordered_map<std::string, callback> callbacks{};
			return callbacks;
		}

		bool handle_command(const command::args& args, const game::netadr_t& from, game::msg_t& msg)
		{
			const auto cmd_string = utils::string::to_lower(args[0]);
			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(cmd_string);

			if (handler == callbacks.end())
			{
				return false;
			}

			const auto msg_backup = msg;
			const auto cb = handler->second(args, from, msg);

			if (msg.readcount != msg_backup.readcount)
				msg = msg_backup;

			return cb;
		}

		bool __fastcall callback_cl_dispatch_connectionless_packet(const game::netadr_t& from, game::msg_t& msg)
		{
			if (const command::args args{}; args.size() > 0)
			{
				if (log_commands)
				{
					PRINT_LOG("Received OOB '%s' from %s", args.join(0).data(), get_sender_string(from).data());
				}

				return handle_command(args, from, msg);
			}

			return false;
		}

		size_t cl_dispatch_connectionless_packet_stub()
		{
			const auto stub = utils::hook::assemble([](auto& a)
			{
				const auto return_original = a.newLabel();

				a.mov(rdx, r12);

				a.pushad64();
				a.mov(rdx, r12); // msg
				a.mov(rcx, r15); // netadr
				a.call_aligned(connectionless_packet::callback_cl_dispatch_connectionless_packet);
				a.test(al, al);
				a.jz(return_original);
				a.popad64();

				a.jmp(game::base_address + 0x134C43F);

				a.bind(return_original);
				a.popad64();
				a.jmp(game::base_address + 0x134BDB0);
			});

			return reinterpret_cast<size_t>(stub);
		}
	}

	void on_command(const std::string& command, const callback& callback)
	{
		get_callbacks()[utils::string::to_lower(command)] = callback;
	}

	void initialize()
	{
		exception::hwbp::register_hook(game::base_address + 0x134BDAD, events::connectionless_packet::cl_dispatch_connectionless_packet_stub);
		
		connectionless_packet::on_command("relay", [](const auto&, const auto& from, const auto&) -> bool
		{
			PRINT_MESSAGE("Connectionless Packet", "Crash attempt caught! from %s", get_sender_string(from).data());
			return true;
		});

		connectionless_packet::on_command("vt", [](const auto&, const auto& from, const auto&) -> bool
		{
			PRINT_MESSAGE("Connectionless Packet", "Crash attempt caught! from %s", get_sender_string(from).data());
			return true;
		});

		connectionless_packet::on_command("requeststats", [](const auto&, const auto& from, const auto&) -> bool
		{
			PRINT_MESSAGE("Connectionless Packet", "Black screen attempt caught! from %s", get_sender_string(from).data());
			return true;
		});

		connectionless_packet::on_command("mstart", [](const auto&, const auto& from, const auto&) -> bool
		{
			PRINT_MESSAGE("Connectionless Packet", "Migration screen attempt caught! from %s", get_sender_string(from).data());
			return true;
		});

		connectionless_packet::on_command("connectResponseMigration", [](const auto&, const auto& from, const auto&) -> bool
		{
			PRINT_MESSAGE("Connectionless Packet", "Kick attempt caught! from %s", get_sender_string(from).data());
			return true;
		});
	}
}