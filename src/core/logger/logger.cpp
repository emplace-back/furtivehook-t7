#include "dependencies/std_include.hpp"
#include "logger.hpp"

namespace logger
{
	std::string get_log_timestamp()
	{
		tm ltime{};
		char timestamp[MAX_PATH] = { 0 };
		const auto time = _time64(nullptr);

		_localtime64_s(&ltime, &time);
		std::strftime(timestamp, sizeof(timestamp) - 1, "%F %X", &ltime);

		return timestamp;
	}

	std::string get_log_message(const std::string& message)
	{
		auto log_message{ "[" + get_log_timestamp() + "]" };
		log_message.push_back(' ');
		log_message.append(message);
		log_message.push_back('\n');
		return log_message;
	}

	void print_log(const char* msg, ...)
	{
		va_list ap;
		va_start(ap, msg);
		const auto result = utils::string::format(ap, msg);
		va_end(ap);

		const static auto filename = utils::string::generate_log_filename("furtivehook/logs/");
		utils::io::write_file(filename, get_log_message(result), true);
	}
}