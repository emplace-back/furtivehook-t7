#include "dependencies/std_include.hpp"
#include "hwbp.hpp"

namespace utils::hwbp
{
	namespace
	{
		class debug_context
		{
		public:
			debug_context(uint32_t thread_id)
				: handle_(thread_id, THREAD_SET_CONTEXT | THREAD_GET_CONTEXT)
			{
				if (!this->handle_)
				{
					throw std::runtime_error("Unable to access thread");
				}

				this->context_.ContextFlags = CONTEXT_DEBUG_REGISTERS;

				if (!GetThreadContext(this->handle_, &this->context_))
				{
					throw std::runtime_error("Unable to get thread context");
				}
			}

			~debug_context()
			{
				SetThreadContext(this->handle_, &this->context_);
			}

			debug_context(const debug_context&) = delete;
			debug_context& operator=(const debug_context&) = delete;

			debug_context(debug_context&& obj) noexcept = delete;
			debug_context& operator=(debug_context&& obj) noexcept = delete;

			operator CONTEXT&()
			{
				return this->context_;
			}

		private:
			utils::thread::handle handle_;
			CONTEXT context_{};
		};
		
		void set_bits(uint64_t& value, const uint32_t bit_index, const uint32_t bits, const uint64_t new_value)
		{
			const auto range_mask = (1ull << bits) - 1ull;
			const auto full_mask = ~(range_mask << bit_index);
			value = (value & full_mask) | (new_value << bit_index);
		}
		
		uint32_t find_free_index(const CONTEXT& ctx)
		{
			for (auto i = 0u; i < sizeof(uint32_t); ++i)
			{
				if ((ctx.Dr7 & (1ull << (i << 1ull))) == 0)
				{
					return i;
				}
			}

			throw std::runtime_error("No free index");
		}
	}

	void activate(const uintptr_t address, const condition cond, const size_t length, CONTEXT& ctx)
	{
		const auto index = hwbp::find_free_index(ctx);
		(&ctx.Dr0)[index] = address;

		hwbp::set_bits(ctx.Dr7, index << 1ull, 1, 1);
		hwbp::set_bits(ctx.Dr7, 16 + (index << 2ull), 2, cond);
		hwbp::set_bits(ctx.Dr7, 18 + (index << 2ull), 2, length - 1);
	}

	void activate(const uintptr_t address, const uint32_t thread_id, const condition cond, const size_t length)
	{
		hwbp::activate(address, cond, length, debug_context{ thread_id });
	}

	void activate(const uintptr_t address, const condition cond, const size_t length)
	{
		const auto thread_ids{ utils::thread::get_thread_ids() };

		for (const auto& thread_id : thread_ids)
			hwbp::activate(address, cond, length, debug_context{ thread_id });
	}

	void deactivate(const uint32_t index, CONTEXT& ctx)
	{
		if (index >= 4)
			throw std::runtime_error("Invalid index");

		hwbp::set_bits(ctx.Dr7, index << 1ull, 1, 0);
	}

	void deactivate(const uint32_t index, const uint32_t thread_id)
	{
		hwbp::deactivate(index, debug_context{ thread_id });
	}

	void deactivate(const uint32_t index)
	{
		const auto thread_ids{ utils::thread::get_thread_ids() };

		for (const auto& thread_id : thread_ids)
			hwbp::deactivate(index, debug_context{ thread_id });
	}

	void deactivate_all(CONTEXT& ctx)
	{
		ctx.Dr7 = 0;
	}

	void deactivate_all(const uint32_t thread_id)
	{
		hwbp::deactivate_all(debug_context{ thread_id });
	}

	void deactivate_all()
	{
		const auto thread_ids{ utils::thread::get_thread_ids() };

		for (const auto& thread_id : thread_ids)
			hwbp::deactivate_all(debug_context{ thread_id });
	}
}