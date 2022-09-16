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
	}

	bool __fastcall handle_command(const game::netadr_t& from)
	{
		const command::args args{};

		if (log_commands)
		{
			PRINT_LOG("Received OOB '%s' from %s", args.join(0).data(), utils::get_sender_string(from).data());
		}

		const auto oob_string = utils::string::to_lower(args[0]);
		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find(oob_string);

		if (handler == callbacks.end())
			return false;

		return handler->second(args, from);
	}

	bool __fastcall cl_dispatch_connectionless_packet_stub(LocalClientNum_t localClientNum, game::netadr_t from, game::msg_t* msg)
	{
		if (events::connectionless_packet::handle_command(from))
			return false;

		return reinterpret_cast<decltype(&cl_dispatch_connectionless_packet_stub)>(game::base_address + 0x134BD50)(localClientNum, from, msg);
	}

	void initialize()
	{
		utils::hook::call(game::base_address + 0x134B838, cl_dispatch_connectionless_packet_stub); 
		
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