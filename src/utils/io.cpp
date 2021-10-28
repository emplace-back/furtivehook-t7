#include "dependencies/std_include.hpp"
#include "io.hpp"

namespace utils::io
{
	bool write_file(const std::string& file, const std::string& data, const bool append)
	{
		if (const auto pos = file.find_last_of("/\\"); pos != std::string::npos)
		{
			create_directory(file.substr(0, pos));
		}

		std::ofstream stream(file, std::ios::binary | std::ofstream::out | (append ? std::ofstream::app : 0));

		if (stream.is_open())
		{
			auto line = ""s;
			line.append(data);

			if (append)
				line.append("\n");

			stream.write(line.data(), line.size());
			stream.close();

			return true;
		}

		return false;
	}
	
	void write_minidump(const LPEXCEPTION_POINTERS ex, const std::string& filename, const std::string& error)
	{
		const auto data = exception::minidump::create_minidump(ex);
		write_file(filename, data);

		if (!error.empty())
		{
			PRINT_LOG("%s", error.data());
		}
	}
	
	bool create_directory(const std::string& directory)
	{
		return std::filesystem::create_directories(directory);
	}

	bool remove_file(const std::string& file)
	{
		return DeleteFileA(file.data()) == TRUE;
	}

	bool move_file(const std::string& src, const std::string& target)
	{
		return MoveFileA(src.data(), target.data()) == TRUE;
	}
	
	bool file_exists(const std::string& file)
	{
		return std::ifstream(file).good();
	}

	std::string get_json_file(const std::string& name)
	{
		const utils::nt::library self;
		return self.get_folder() + "\\furtivehook\\json\\" + name;
	}

	std::ifstream read_json_file(const std::string& name)
	{
		if (const auto file = get_json_file(name); file_exists(file))
		{
			return std::ifstream(file);
		}

		return {};
	}

	json parse_json_file(const std::string& name)
	{
		auto json_file = read_json_file(name);
		return json::parse(json_file);
	}
}