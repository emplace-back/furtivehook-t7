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
		MH_EnableHook(this->place_);

		if (!this->moved_data_.empty())
		{
			this->move();
		}
	}

	void detour::disable()
	{
		this->un_move();
		MH_DisableHook(this->place_);
	}

	void detour::create(void* place, void* target)
	{
		this->clear();
		this->place_ = place;

		if (MH_CreateHook(this->place_, target, &this->original_) != MH_OK)
		{
			throw std::runtime_error(string::va("Unable to create hook at location: 0x%llX", this->place_));
		}

		this->enable();
	}

	void detour::create(const size_t place, void* target)
	{
		this->create(reinterpret_cast<void*>(place), target);
	}

	void detour::clear()
	{
		if (this->place_)
		{
			this->un_move();
			MH_RemoveHook(this->place_);
		}

		this->place_ = nullptr;
		this->original_ = nullptr;
		this->moved_data_ = {};
	}

	void detour::move()
	{
		this->moved_data_ = move_hook(this->place_);
	}

	void* detour::get_original() const
	{
		return this->original_;
	}

	void detour::un_move()
	{
		if (!this->moved_data_.empty())
		{
			copy(this->place_, this->moved_data_.data(), this->moved_data_.size());
		}
	}

	void write_string(char* place, const std::string& string)
	{
		std::strncpy(place, string.data(), string.size());
		place[string.size()] = 0;
	}
	
	void retn(const uintptr_t address)
	{
		set(address, instr::ret);
	}
	
	void nop(const uintptr_t address, const size_t size)
	{
		DWORD old_protect{ 0 };
		VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old_protect);

		std::memset(reinterpret_cast<void*>(address), std::uint8_t(instr::nop), size);

		VirtualProtect(reinterpret_cast<void*>(address), size, old_protect, &old_protect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), size);
	}

	std::vector<uint8_t> move_hook(void* pointer)
	{
		std::vector<uint8_t> original_data{};

		auto* data_ptr = static_cast<uint8_t*>(pointer);
		if (data_ptr[0] == 0xE9)
		{
			original_data.resize(6);
			std::memmove(original_data.data(), pointer, original_data.size());

			auto* target = follow_branch(data_ptr);
			nop(uintptr_t(data_ptr), 1);
			jump(data_ptr + 1, target);
		}
		else if (data_ptr[0] == 0xFF && data_ptr[1] == 0x25)
		{
			original_data.resize(15);
			std::memmove(original_data.data(), pointer, original_data.size());

			copy(data_ptr + 1, data_ptr, 14);
			nop(uintptr_t(data_ptr), 1);
		}
		else
		{
			throw std::runtime_error("No branch instruction found");
		}

		return original_data;
	}

	std::vector<uint8_t> move_hook(const size_t pointer)
	{
		return move_hook(reinterpret_cast<void*>(pointer));
	}

	void* follow_branch(void* address)
	{
		auto* const data = static_cast<uint8_t*>(address);
		if (*data != 0xE8 && *data != 0xE9)
		{
			throw std::runtime_error("No branch instruction found");
		}

		return extract<void*>(data + 1);
	}
}