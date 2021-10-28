#pragma once
#include "dependencies/std_include.hpp"

namespace utils::nt
{
	class library final
	{
	public:
		static library load(const std::string& name);
		static library load(const std::filesystem::path& path);
		static library get_by_address(void* address);

		library();
		explicit library(const std::string& name);
		explicit library(HMODULE handle);

		library(const library& a) : module_(a.module_)
		{
		}

		bool operator!=(const library& obj) const noexcept { return !(*this == obj); };
		bool operator==(const library& obj) const noexcept;

		operator bool() const noexcept;
		operator HMODULE() const noexcept;

		void unprotect() const noexcept;
		void* get_entry_point() const noexcept;
		size_t get_base_address() const noexcept;
		size_t get_relative_entry_point() const noexcept;

		bool is_valid() const noexcept;
		std::string get_name() const noexcept;
		std::string get_path() const noexcept;
		std::string get_folder() const noexcept;
		std::uint8_t* get_ptr() const noexcept;
		void free();

		HMODULE get_handle() const noexcept;

		template <typename T>
		T get_proc(const std::string& process) const
		{
			if (!this->is_valid()) T{};
			return reinterpret_cast<T>(GetProcAddress(this->module_, process.data()));
		}

		template <typename T>
		std::function<T> get(const std::string& process) const
		{
			if (!this->is_valid()) return std::function<T>();
			return static_cast<T*>(this->get_proc<void*>(process));
		}

		template <typename T, typename... Args>
		T invoke(const std::string& process, Args ... args) const
		{
			auto method = this->get<T(__cdecl)(Args ...)>(process);
			if (method) return method(args...);
			return T();
		}

		template <typename T, typename... Args>
		T invoke_pascal(const std::string& process, Args ... args) const
		{
			auto method = this->get<T(__stdcall)(Args ...)>(process);
			if (method) return method(args...);
			return T();
		}

		template <typename T, typename... Args>
		T invoke_this(const std::string& process, void* this_ptr, Args ... args) const
		{
			auto method = this->get<T(__thiscall)(void*, Args ...)>(this_ptr, process);
			if (method) return method(args...);
			return T();
		}

		std::vector<PIMAGE_SECTION_HEADER> get_section_headers() const noexcept;

		PIMAGE_NT_HEADERS get_nt_headers() const noexcept;
		PIMAGE_DOS_HEADER get_dos_header() const noexcept;
		PIMAGE_OPTIONAL_HEADER get_optional_header() const noexcept;

		void** get_iat_entry(const std::string& module_name, const std::string& proc_name) const noexcept;

	private:
		HMODULE module_;
	};

	__declspec(noreturn) void raise_hard_exception();
	std::string load_resource(int id);

	void relaunch_self();
	__declspec(noreturn) void terminate(uint32_t code = 0);
}