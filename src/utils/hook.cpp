#include "dependencies/std_include.hpp"
#include "hook.hpp"

namespace utils::hook
{
	namespace
	{
		[[maybe_unused]] class _
		{
		public:
			_()
			{
				if (MH_Initialize() != MH_OK)
				{
					throw std::runtime_error("Failed to initialize MinHook");
				}
			}

			~_()
			{
				MH_Uninitialize();
			}
		} __;

		void* follow_branch(void* address)
		{
			const auto data = static_cast<uint8_t*>(address);

			if (*data != 0xE8 && *data != 0xE9)
			{
				throw std::runtime_error("No branch instruction found");
			}

			return extract<void*>(data + 1);
		}
	}

	detour::detour(const size_t place, void* target) : detour(reinterpret_cast<void*>(place), target)
	{
	}

	detour::detour(void* place, void* target)
	{
		this->create(place, target);
	}

	detour::~detour()
	{
		this->clear();
	}

	void detour::enable()
	{
		MH_EnableHook(this->place);

		if (!this->moved_data.empty())
		{
			this->move();
		}
	}

	void detour::disable()
	{
		this->un_move();
		MH_DisableHook(this->place);
	}

	void detour::create(void* place, void* target)
	{
		this->clear();
		this->place = place;

		if (MH_CreateHook(this->place, target, &this->original) != MH_OK)
		{
			throw std::runtime_error(string::va("Unable to create hook at location: 0x%llX", this->place));
		}

		this->enable();
	}

	void detour::create(const size_t place, void* target)
	{
		this->create(reinterpret_cast<void*>(place), target);
	}

	void detour::clear()
	{
		if (this->place)
		{
			this->un_move();
			MH_RemoveHook(this->place);
		}

		this->place = nullptr;
		this->original = nullptr;
		this->moved_data = {};
	}

	void detour::move()
	{
		this->moved_data = move_hook(this->place);
	}

	void detour::un_move()
	{
		if (!this->moved_data.empty())
		{
			copy(this->place, this->moved_data.data(), this->moved_data.size());
		}
	}

	void write(char* place, const std::string& string)
	{
		std::strncpy(place, string.data(), string.size());
		place[string.size()] = 0;
	}
	
	void nop(const uintptr_t address, const size_t size)
	{
		DWORD old_protect{ 0 };
		VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old_protect);

		std::memset(reinterpret_cast<void*>(address), std::uint8_t(instr::nop), size);

		VirtualProtect(reinterpret_cast<void*>(address), size, old_protect, &old_protect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), size);
	}

	void nop(const void* place, const size_t size)
	{
		return nop(uintptr_t(place), size);
	}

	std::vector<uint8_t> move_hook(const uintptr_t address)
	{
		std::vector<uint8_t> original_data{};

		const auto data = reinterpret_cast<uint8_t*>(address);

		if (*data == 0xE9)
		{
			original_data.insert(original_data.begin(), data, data + 6);

			const auto target = follow_branch(data);
			nop(data, 1);
			jump(data + 1, target);
		}
		else if (*data == 0xFF && data[1] == 0x25)
		{
			original_data.insert(original_data.begin(), data, data + 15);

			copy(data + 1, data, 14);
			nop(data, 1);
		}
		else
		{
			throw std::runtime_error("No branch instruction found");
		}

		return original_data;
	}

	std::vector<uint8_t> move_hook(const void* pointer)
	{
		return move_hook(uintptr_t(pointer));
	}
}