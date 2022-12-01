#pragma once

namespace command
{
	class args final
	{
	public:
		args() : cmd_args(game::Sys_GetTLS()->cmdArgs) {}

		args(const game::CmdArgs* a) : cmd_args(a)
		{
		}

		const char* get(const int index) const;
		int size() const;
		std::string join(const int index = 0) const;

		const char* operator[](const int index) const
		{
			return this->get(index);
		}
	private:
		const game::CmdArgs* cmd_args{};
	};
	
	using callback = std::function<void(const args&)>;
	void execute(std::string command, const bool sync = false);
}
