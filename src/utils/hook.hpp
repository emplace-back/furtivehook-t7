#pragma once
#include "dependencies/std_include.hpp"

#include <utils/asmjit/core/jitruntime.h>
#include <utils/asmjit/x86/x86assembler.h>

using namespace asmjit::x86;

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

	class assembler : public Assembler
	{
	public:
		using Assembler::Assembler;
		using Assembler::call;
		using Assembler::jmp;

		void pushad64();
		void popad64(const bool ret_val = false);

		void prepare_stack_for_call();
		void restore_stack_after_call();

		template <typename T>
		void call_aligned(T&& target)
		{
			this->prepare_stack_for_call();
			this->call(std::forward<T>(target));
			this->restore_stack_after_call();
		}

		asmjit::Error call(void* target);
		asmjit::Error jmp(void* target);
	};
	
	class detour
	{
	public:
		detour() = default;
		detour(void* place, void* target);
		detour(uintptr_t place, void* target);
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

				this->place = other.place;
				this->original = other.original;
				this->moved_data = other.moved_data;

				other.place = nullptr;
				other.original = nullptr;
				other.moved_data = {};
			}

			return *this;
		}

		detour(const detour&) = delete;
		detour& operator=(const detour&) = delete;

		void enable();
		void disable();

		void create(void* place, void* target);
		void create(uintptr_t place, void* target);
		void clear();
		void move();

		void* get_original()
		{
			return this->original;
		}

		template <typename T>
		T* get() const
		{
			return static_cast<T*>(this->original);
		}

		template <typename T = void, typename... Args>
		T call(Args ... args)
		{
			return static_cast<T(*)(Args ...)>(this->original)(args...);
		}

		template <typename T = void, typename... Args>
		T call_spoofed(Args ... args)
		{
			return spoof_call(static_cast<T(*)(Args ...)>(this->original), args...);
		}
	private:
		std::vector<uint8_t> moved_data{};
		void* place{};
		void* original{};

		void un_move();
	};

	void* get_memory_near(const uintptr_t address, const size_t size);
	uint8_t* allocate_somewhere_near(const uintptr_t base_address, const size_t size);
	void retn(const uintptr_t address);
	void return_value(const uintptr_t address, const int value = 0);
	void nop(const uintptr_t address, const size_t length);
	void nop(const void* place, const size_t length);
	void copy(const uintptr_t address, const void* data, const size_t length);
	void copy(const void* place, const void* data, const size_t length);
	bool is_relatively_far(const uintptr_t address, const void* function, const int offset = 5);
	void jump(const uintptr_t address, const void* function, const bool use_far = false, const bool use_safe = false);
	void jump(const void* place, const void* function, const bool use_far = false, const bool use_safe = false);
	void jump(const uintptr_t address, const uintptr_t function, const bool use_far = false, const bool use_safe = false);
	void call(const uintptr_t address, const void* function);
	std::vector<uint8_t> move_hook(const uintptr_t address);
	std::vector<uint8_t> move_hook(const void* pointer);
	void* assemble(const std::function<void(assembler&)>& asm_function);
	void* follow_branch(void* address);
	
	template <typename T> void set(const uintptr_t address, const T value)
	{
		copy(address, &value, sizeof(value));
	}

	template <typename T> void set(const void* place, const T value)
	{
		set<T>(uintptr_t(place), value);
	}

	template <typename T> auto iat(const std::string& mod_name, const std::string& proc_name, T function)
	{
		const nt::library main{};
		if (!main.is_valid()) return T();

		auto ptr = main.get_iat_entry(mod_name, proc_name);
		if (!ptr) return T();

		auto original = *ptr;
		set(ptr, uintptr_t(function));
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

	template <typename T> T extract(void* place)
	{
		const auto data = static_cast<uint8_t*>(place);
		const auto offset = *reinterpret_cast<int*>(data);
		
		return reinterpret_cast<T>(data + offset + 4);
	}
}