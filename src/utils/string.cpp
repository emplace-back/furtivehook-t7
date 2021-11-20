#include "dependencies/std_include.hpp"
#include "string.hpp"

namespace utils::string
{
	std::string adr_to_string(const game::XNADDR* address)
	{
		static auto adr = ""s;
		adr = utils::string::va("%u.%u.%u.%u:%u", address->ip[0], address->ip[1], address->ip[2], address->ip[3], address->port);

		return adr;
	}

	std::string adr_to_string(const game::netadr_t* address)
	{
		game::XNADDR xn;
		game::dwNetadrToCommonAddr(*address, &xn, sizeof xn, nullptr);

		if (address->type == game::NA_LOOPBACK)
			return utils::string::va("%u.%u.%u.%u", xn.ip[0], xn.ip[1], xn.ip[2], xn.ip[3]);

		return utils::string::va("%u.%u.%u.%u:%u", xn.ip[0], xn.ip[1], xn.ip[2], xn.ip[3], address->port);
	}
	
	std::string join(const std::vector<std::string>& args, const std::size_t index)
	{
		auto result = ""s;

		for (auto i = index; i < args.size(); ++i)
		{
			if (i > index)
			{
				result.append(" ");
			}

			result.append(args[i]);
		}

		return result;
	}
	
	std::string dump_hex(const std::string& data, const std::string& separator)
	{
		auto result = ""s;

		for (size_t i = 0; i < data.size(); ++i)
		{
			if (i > 0)
			{
				result.append(separator);
			}

			result.append(va("%02X", data[i] & 0xFF));
		}

		return result;
	}
	
	std::string va(const char* fmt, ...)
	{
		static thread_local va_provider<8, 256> provider;

		va_list ap;
		va_start(ap, fmt);

		const auto* result = provider.get(fmt, ap);

		va_end(ap);
		return result;
	}

	std::vector<std::string> split(const std::string& text, const char delimiter)
	{
		std::stringstream ss(text);
		std::string item;
		std::vector<std::string> elems;

		while (std::getline(ss, item, delimiter))
		{
			elems.emplace_back(item);
		}

		return elems;
	}

	std::string replace_all(std::string str, const std::string& from, const std::string& to)
	{
		if (from.empty())
		{
			return str;
		}
		
		std::string::size_type start_pos = 0;

		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}

		return str;
	}

	bool begins_with(const std::string& text, const std::string& substring)
	{
		return text.find(substring) == 0;
	}

	bool contains(const std::string &text, const std::string &substring)
	{
		return text.find(substring) != std::string::npos;
	}

	bool ends_with(const std::string& text, const std::string& substring)
	{
		if (substring.size() > text.size()) return false;
		return std::equal(substring.rbegin(), substring.rend(), text.rbegin());
	}

	std::string to_lower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const auto& input) { return static_cast<char>(std::tolower(input)); });
		return text;
	}

	std::string to_upper(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const auto& input) { return static_cast<char>(std::toupper(input)); });
		return text;
	}

	std::string reverse_words(std::string_view s)
	{
		std::string result;
		result.reserve(s.size());

		while (!s.empty())
		{
			const auto i = s.rfind(' ');
			result.append(s.begin() + i + 1, s.end());

			if (i == std::string_view::npos) break;

			s = s.substr(0, i);
		}

		return result;
	}

	std::string generate_random_string(const size_t length)
	{
		const auto random_characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"s;
		auto result = ""s;

		std::random_device device;
		std::mt19937 engine(device());
		std::uniform_int_distribution<std::uint32_t> dist(0, random_characters.size() - 1);

		for (auto i = 0u; i != length; ++i)
		{
			const auto random_index = dist(engine);
			result += random_characters[random_index];
		}

		return result;
	};
}