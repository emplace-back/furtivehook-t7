#pragma once

namespace command
{
	class args_ final
	{
	public:
		const char* get(const int index) const noexcept;
		int size() const noexcept;
		std::string join(const int index = 0) const noexcept;

		const char* operator[](const int index) const
		{
			return this->get(index);
		}
	}extern args;
}
