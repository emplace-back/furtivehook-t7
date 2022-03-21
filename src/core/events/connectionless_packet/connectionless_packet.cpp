#include "dependencies/std_include.hpp"
#include "connectionless_packet.hpp"

namespace events::connectionless_packet
{
	bool log_commands = true;

	namespace
	{
		std::string get_sender_string(const game::netadr_t& from)
		{
			const auto client_num = game::find_target_from_addr(from);

			if (client_num < 0)
			{
				return utils::string::va("%s", utils::string::adr_to_string(&from).data());
			}
			
			const auto client = game::session->clients[client_num].activeClient;
			return utils::string::va("'%s' (%llu) %s", client->fixedClientInfo.gamertag, client->fixedClientInfo.xuid, utils::string::adr_to_string(&from).data());
		}

		using callback = std::function<bool(const command::args&, const game::netadr_t&, game::msg_t&)>;
		
		std::unordered_map<std::string, callback>& get_callbacks()
		{
			static std::unordered_map<std::string, callback> callbacks{};
			return callbacks;
		}

		void on_command(const std::string& command, const callback& callback)
		{
			get_callbacks()[utils::string::to_lower(command)] = callback;
		}

		bool handle_command(const command::args& args, const game::netadr_t& from, game::msg_t& msg)
		{
			const auto oob_string = utils::string::to_lower(args[0]);
			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(oob_string);

			if (handler == callbacks.end())
			{
				return false;
			}

			const auto msg_backup = msg;
			const auto callback = handler->second(args, from, msg);

			if (msg.readcount != msg_backup.readcount)
				msg = msg_backup;

			return callback;
		}

		bool __fastcall callback_cl_dispatch_connectionless_packet(const game::netadr_t& from, game::msg_t& msg)
		{
			if (const command::args args{}; args.size() > 0)
			{
				if (log_commands)
				{
					PRINT_LOG("Received OOB '%s' from %s", args.join(0).data(), get_sender_string(from).data());
				}

				const auto handled = handle_command(args, from, msg);

				if (handled)
				{
					PRINT_LOG("Ignoring OOB '%s' from %s", args.join(0).data(), get_sender_string(from).data());
				}

				return handled;
			}

			return false;
		}
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

	void initialize()
	{
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