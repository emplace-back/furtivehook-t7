#pragma once
#include "dependencies/std_include.hpp"

#include <utils/asmjit/core/jitruntime.h>
#include <utils/asmjit/x86/x86assembler.h>

using namespace asmjit::x86;

namespace utils::hook
{
	enum class instructions
	{ 
		call = 0xE8, 
		jump = 0xE9,
		retn = 0xC3,
		mov_al_1_ret = 0xC301B0,
		xor_eax_eax_ret = 0xC3C033,
		nop = 0x90,
	};
	
	class assembler : public Assembler
	{
	public:
		using Assembler::Assembler;
		using Assembler::call;
		using Assembler::jmp;

		void pushad64();
		void popad64();

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

				other.place_ = nullptr;
				other.original_ = nullptr;
			}

			return *this;
		}

		detour(const detour&) = delete;
		detour& operator=(const detour&) = delete;

		void enable() const;
		void disable() const;

		void create(void* place, void* target);
		void create(size_t place, void* target);
		void clear();

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
		void* place_{};
		void* original_{};
	};

	void return_value(void * place, bool value);
	
	bool is_relatively_far(const void* pointer, const void* data, int offset = 5); 
	
	void jump(void* pointer, void* data, bool use_far = false);
	void jump(size_t pointer, void* data, bool use_far = false);
	void jump(size_t pointer, size_t data, bool use_far = false);
	
	void call(void* pointer, void* data);
	void call(size_t pointer, void* data);
	void call(size_t pointer, size_t data);
	void* assemble(const std::function<void(assembler&)>& asm_function);
	
	void nop(void* place, size_t length = 5);

	inline void write_string(char* place, const std::string& string)
	{
		if (string == place)
		{
			return;
		}
		
		DWORD old_protect;
		VirtualProtect(place, sizeof(place), PAGE_EXECUTE_READWRITE, &old_protect);
		
		std::strncpy(place, string.data(), string.size());
		place[string.size()] = 0;

		VirtualProtect(place, sizeof(place), old_protect, &old_protect);
		FlushInstructionCache(GetCurrentProcess(), place, sizeof(place));
	}
	
	inline void return_value(const std::size_t place, bool value)
	{
		return_value(reinterpret_cast<void*>(place), value);
	}

	inline void nop(const std::size_t place, size_t length = 5)
	{
		nop(reinterpret_cast<void*>(place), length);
	}

	template <typename T>
	static void set(void* place, T value)
	{
		DWORD old_protect;
		VirtualProtect(place, sizeof(T), PAGE_EXECUTE_READWRITE, &old_protect);

		*static_cast<T*>(place) = value;

		VirtualProtect(place, sizeof(T), old_protect, &old_protect);
		FlushInstructionCache(GetCurrentProcess(), place, sizeof(T));
	}

	template <typename T>
	static void set(const size_t place, T value)
	{
		return set<T>(reinterpret_cast<void*>(place), value);
	}

	static void retn(const std::size_t place)
	{
		return set<std::uint8_t>(place, static_cast<std::uint8_t>(instructions::retn));
	}
}