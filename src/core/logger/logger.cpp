#include "dependencies/std_include.hpp"
#include "logger.hpp"

namespace logger
{
	void add_to_console(const char* msg, ...)
	{
		char buffer[2048];

		va_list ap;
		va_start(ap, msg);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

		va_end(ap);

		menu::console_items.emplace_back(buffer);
	}
	
	void print_log(const char* msg, ...)
	{
		char buffer[2048];

		va_list ap;
		va_start(ap, msg);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

		va_end(ap);

		const static auto filename = utils::generate_log_filename("furtivehook\\logs\\");
		utils::io::write_file(filename, buffer, true);
	}

	void print_sv_log(const char* msg, ...)
	{
		char buffer[2048];

		va_list ap;
		va_start(ap, msg);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

		va_end(ap);

		const static auto filename = utils::generate_log_filename("furtivehook\\logs\\server\\");
		utils::io::write_file(filename, buffer, true);
	}
}