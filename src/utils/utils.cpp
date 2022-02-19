#include "dependencies/std_include.hpp"
#include "utils.hpp"

namespace utils
{
	std::string get_log_file_timestamp()
	{
		tm ltime{};
		char timestamp[MAX_PATH] = { 0 };
		const auto time = _time64(nullptr);

		_localtime64_s(&ltime, &time);
		std::strftime(timestamp, sizeof(timestamp) - 1, "%F", &ltime);

		return timestamp;
	}
	
	std::string generate_log_filename(const std::string& dir, const std::string& ext)
	{
		const auto timestamp = utils::get_log_file_timestamp();
		const auto filename = utils::string::va("furtivehook-%s.%s", timestamp.data(), ext.data());

		return dir + filename;
	}
}