#include "dependencies/std_include.hpp"
#include "connectionless_packet.hpp"

namespace events::connectionless_packet
{
	bool log_commands = true;

	namespace
	{
		using callback = std::function<bool(const command::args&, const game::netadr_t&)>;
		
		std::unordered_map<std::string, callback>& get_callbacks()
		{
			static std::unordered_map<std::string, callback> callbacks{};
			return callbacks;
		}

		void on_command(const std::string& command, const callback& callback)
		{
			get_callbacks()[utils::string::to_lower(command)] = callback;
		}

		bool __fastcall handle_command(const game::netadr_t& from, const bool sv = false)
		{
			const auto args = sv ? command::args::get_server() : command::args::get_client();

			if (log_commands)
			{
				PRINT_LOG("Received OOB [%s] '%s' from %s",
					sv ? "s" : "c",
					args.join(0).data(),
					utils::get_sender_string(from).data());
			}

			const auto oob_string = utils::string::to_lower(args[0]);
			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(oob_string);

			if (handler == callbacks.end())
				return false;

			return handler->second(args, from);
		}

		bool __fastcall cl_dispatch_connectionless_packet(LocalClientNum_t localClientNum, game::netadr_t from, game::msg_t* msg)
		{
			if (connectionless_packet::handle_command(from))
				return false;

			return game::call<bool>(game::base_address + 0x134BD50, localClientNum, from, msg);
		}

		bool __fastcall sv_connectionless_packet(game::netadr_t& from, const char* message)
		{
			auto args = command::args::get_server();
			args.tokenize(message, true);

			const auto handled = connectionless_packet::handle_command(from, true);

			if (handled)
			{
				args.end_tokenize();
				args.tokenize("");
			}

			return handled;
		}
	}

	void initialize()
	{
		const auto sv_connectionless_packet_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.pushad64();
			a.mov(rdx, rax);
			a.mov(rcx, rdi);
			a.call_aligned(connectionless_packet::sv_connectionless_packet);
			a.popad64();

			a.jmp(game::base_address + 0x22538C9);
		}); 
		
		utils::hook::jump(game::base_address + 0x22538C4, sv_connectionless_packet_stub);
		utils::hook::call(game::base_address + 0x134B838, cl_dispatch_connectionless_packet);
		utils::hook::return_value(game::base_address + 0x1358310, 0); //CL_HandleVoiceTypePacket
		
		const auto crash_attempt_oob = [](const command::args&, const game::netadr_t& from)
		{
			PRINT_MESSAGE("Connectionless Packet", "Crash attempt caught! from %s", utils::get_sender_string(from).data());
			return true;
		}; 
		
		const auto ignore_oob = [](const command::args& args, const game::netadr_t& from)
		{
			PRINT_LOG("Ignoring OOB '%s' from %s", args.join(0).data(), utils::get_sender_string(from).data());
			return true;
		};
		
		connectionless_packet::on_command("requeststats", ignore_oob);
		connectionless_packet::on_command("mstart", ignore_oob);
		connectionless_packet::on_command("mhead", ignore_oob);
		connectionless_packet::on_command("mstate", ignore_oob);
		connectionless_packet::on_command("connectResponseMigration", ignore_oob);
		connectionless_packet::on_command("print", ignore_oob);
		connectionless_packet::on_command("echo", ignore_oob);
		connectionless_packet::on_command("statusResponse", ignore_oob);
		connectionless_packet::on_command("rcon", ignore_oob);
		connectionless_packet::on_command("RA", ignore_oob);
		connectionless_packet::on_command("relay", crash_attempt_oob);
		connectionless_packet::on_command("vt", crash_attempt_oob);
	}
}