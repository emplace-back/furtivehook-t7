#pragma once
#include "dependencies/std_include.hpp"

namespace command
{
	class args final
	{
	public:
		args(game::CmdArgs* a) : cmd_args(a)
		{
			assert(cmd_args && cmd_args->nesting < 8);
		}

		static args get_client();
		static args get_server();

		void tokenize(const char* string, const bool eval_expressions = false) const;
		void end_tokenize() const;
		const char* get(const int index) const;
		int size() const;
		std::string join(const int index = 0) const;

		const char* operator[](const int index) const
		{
			return this->get(index);
		}
	private:
		game::CmdArgs* cmd_args{};
	};
	
	void execute(std::string command, const bool sync = false);
	void initialize();
}
