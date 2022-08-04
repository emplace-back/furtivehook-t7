#pragma once
#include "dependencies/std_include.hpp"

namespace utils::hook
{
	enum class instr : uint8_t
	{
		call = 0xE8,
		jmp = 0xE9,
		ret = 0xC3,
		mov = 0xB8,
		nop = 0x90,
	};

	class detour
	{
	public:
		detour() = default;
		detour(void* place, void* target);
		detour(size_t place, void* target);
		~detour();

		detour(detour&& other) noexcept
		{
			this->operator=(std::move(other));
		}

		detour& operator=(detour&& other) noexcept
		{
			if (this != &other)
			{
				this->~detour();

				this->place_ = other.place_;
				this->original_ = other.original_;
				this->moved_data_ = other.moved_data_;

				other.place_ = nullptr;
				other.original_ = nullptr;
				other.moved_data_ = {};
			}

			return *this;
		}

		detour(const detour&) = delete;
		detour& operator=(const detour&) = delete;

		void enable();
		void disable();

		void create(void* place, void* target);
		void create(size_t place, void* target);
		void clear();
		void move();

		template <typename T>
		T* get() const
		{
			return static_cast<T*>(this->get_original());
		}

		template <typename T = void, typename... Args>
		T invoke(Args ... args)
		{
			return static_cast<T(*)(Args ...)>(this->get_original())(args...);
		}

		[[nodiscard]] void* get_original() const;
	private:
		std::vector<uint8_t> moved_data_{};
		void* place_{};
		void* original_{};

		void un_move();
	};

	void* follow_branch(void* address); 
	void write_string(char* place, const std::string& string);
	void retn(const uintptr_t address);
	void nop(const uintptr_t address, const size_t size);
	std::vector<uint8_t> move_hook(void* pointer);
	std::vector<uint8_t> move_hook(size_t pointer);

	template <typename T> void copy(const uintptr_t address, const T data, const size_t length)
	{
		DWORD old_protect{ 0 };
		VirtualProtect(reinterpret_cast<void*>(address), length, PAGE_EXECUTE_READWRITE, &old_protect);

		std::memmove(reinterpret_cast<void*>(address), data, length);

		VirtualProtect(reinterpret_cast<void*>(address), length, old_protect, &old_protect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), length);
	}
	
	template <typename T> void copy(const void* place, const T data, const size_t length)
	{
		copy<T>(uintptr_t(place), data, length);
	}
	
	template <typename T> void set(const uintptr_t address, const T value)
	{
		DWORD old_protect{ 0 };
		VirtualProtect(reinterpret_cast<void*>(address), sizeof value, PAGE_EXECUTE_READWRITE, &old_protect);

		*reinterpret_cast<T*>(address) = value;

		VirtualProtect(reinterpret_cast<void*>(address), sizeof value, old_protect, &old_protect);

		FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(address), sizeof value);
	}

	template <typename T> void set(const void* place, const T value)
	{
		set<T>(uintptr_t(place), value);
	}

	template <typename T> void jump(const uintptr_t address, const T function, const bool use_far = false, const bool use_safe = false)
	{
		if (use_far)
		{
			const static uint8_t jump_data[] =
			{
				0x48, 0xb8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xff, 0xe0
			}; 

			const static uint8_t jump_data_safe[] =
			{
				0xFF, 0x25, 0x00, 0x00, 0x00, 0x00
			};

			const auto data = use_safe ? jump_data_safe : jump_data;
			const auto jump_size = use_safe ? sizeof(jump_data_safe) : 2;

			copy(address, data, sizeof(data));
			copy(address + jump_size, &function, sizeof(function));
		}
		else
		{
			set(address, instr::jmp);
			set<uint32_t>(address + 1, uintptr_t(function) - address - 5);
		}
	}

	template <typename T> void jump(const void* place, const T function, const bool use_far = false, const bool use_safe = false)
	{
		jump<T>(uintptr_t(place), function, use_far, use_safe);
	}

	template <typename T> void return_value(const uintptr_t address, const T value)
	{
		set(address, instr::mov);
		set(address + 1, uintptr_t(value));
		set(address + 5, instr::ret);
	}

	template <typename T> auto iat(const std::string& mod_name, const std::string& proc_name, T function)
	{
		const nt::library main;
		if (!main.is_valid()) return T();

		auto ptr = main.get_iat_entry(mod_name, proc_name);
		if (!ptr) return T();

		auto original = *ptr;
		set(ptr, reinterpret_cast<uintptr_t*>(function));
		return reinterpret_cast<T>(original);
	}

	template<typename T> T vtable(const uintptr_t address, const size_t index, T function)
	{
		auto table = *reinterpret_cast<uintptr_t**>(address);

		MEMORY_BASIC_INFORMATION mbi{};
		VirtualQuery(reinterpret_cast<void*>(table), &mbi, sizeof(mbi));
		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect);

		const auto original = table[index];
		table[index] = uintptr_t(function);

		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect);

		return reinterpret_cast<T>(original);
	}

	template <typename T> T vtable(const void* place, const size_t index, T function)
	{
		return vtable<T>(uintptr_t(place), index, function);
	}

	template <typename T>
	T extract(void* address)
	{
		auto* const data = static_cast<uint8_t*>(address);
		const auto offset = *reinterpret_cast<int32_t*>(data);
		return reinterpret_cast<T>(data + offset + 4);
	}
}