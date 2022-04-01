#pragma once

namespace command
{
	class args final
	{
	public:
		args() : cmd_args(game::Sys_GetTLS()->cmdArgs) {}
		
		const char* get(const int index) const noexcept;
		int size() const noexcept;
		std::string join(const int index = 0) const noexcept;

		const char* operator[](const int index) const noexcept
		{
			return this->get(index);
		}
	private:
		game::CmdArgs* cmd_args{};
	};
	
	using callback = std::function<void(const args&)>;
	void execute(std::string command, const bool sync = false);
	void initialize();
}
