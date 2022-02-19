#pragma once

namespace command
{
	class args final
	{
	public:
		const char* get(const size_t index) const noexcept;
		int size() const noexcept;
		std::string join(const size_t index = 0) const noexcept;

		const char* operator[](const int index) const
		{
			return this->get(index);
		}
	};
	
	using callback = std::function<void(const args&)>;
	void execute(std::string command, const bool sync = false);
	void initialize();
}
