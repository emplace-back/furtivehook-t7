#include "dependencies/std_include.hpp"
#include "hwbp.hpp"

namespace exception::hwbp
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

			CONTEXT* operator->()
			{
				return &this->context_;
			}

		private:
			utils::thread::handle handle_;
			CONTEXT context_{};
		}; 
		
		using callback = std::function<void(CONTEXT&)>;
		
		std::unordered_map<std::uintptr_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uintptr_t, callback> callbacks{};
			return callbacks;
		}
		
		void set_bits(uint64_t& value, const uint32_t bit_index, const uint32_t bits, const uint64_t new_value)
		{
			const auto range_mask = (1ull << bits) - 1ull;
			const auto full_mask = ~(range_mask << bit_index);
			value = (value & full_mask) | (new_value << bit_index);
		}
		
		uint32_t find_free_index(debug_context& ctx)
		{
			for (auto i = 0; i < sizeof(uint32_t); ++i)
			{
				if ((ctx->Dr7 & (1ull << (i << 1ull))) == 0)
				{
					return i;
				}
			}

			throw std::runtime_error("No free index");
		}

		void register_hook(const std::uintptr_t address, const callback& callback, const condition cond = condition::execute, size_t length = sizeof uint8_t)
		{
			const auto ids{ utils::thread::get_thread_ids() };

			for (const auto& id : ids)
			{
				debug_context ctx(id);

				const auto index = hwbp::find_free_index(ctx);
				(&ctx->Dr0)[index] = address;

				hwbp::set_bits(ctx->Dr7, index << 1ull, 1, 1);
				hwbp::set_bits(ctx->Dr7, 16 + (index << 2ull), 2, cond);
				hwbp::set_bits(ctx->Dr7, 18 + (index << 2ull), 2, length - 1);
			}

			get_callbacks()[address] = callback;
		}
	}
	
	bool handle_exception(const LPEXCEPTION_POINTERS ex)
	{
		if (ex->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP)
			return false; 
		
		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find(ex->ContextRecord->Rip);

		if (handler == callbacks.end())
		{
			return false;
		}

		handler->second(*ex->ContextRecord);
		return true;
	}
	
	void initialize()
	{
		hwbp::register_hook(game::base_address + 0x1EF5610, [](auto& ctx)
		{
			ctx.Rip = reinterpret_cast<size_t>(game::LobbyMsgRW_PackageElement);
		});

		hwbp::register_hook(game::base_address + 0x1439600, [](auto& ctx)
		{
			ctx.Rip = reinterpret_cast<size_t>(events::instant_message::dispatch_message);
		});

		hwbp::register_hook(game::base_address + 0x134BDAD, [](auto& ctx)
		{
			if (events::connectionless_packet::handle_command(*reinterpret_cast<game::netadr_t*>(ctx.R15)))
			{
				ctx.Rip = game::base_address + 0x134C43F;
			}
			else
			{
				ctx.Rdx = ctx.R12;
				ctx.Rip += 0x3;
			}
		});

		hwbp::register_hook(game::base_address + 0x131ED33, [](auto& ctx)
		{
			if (events::server_command::handle_command())
			{
				ctx.Rip = game::base_address + 0x131EEDB;
			}
			else
			{
				ctx.Rbx = *reinterpret_cast<uint64_t*>(ctx.Rax);
				ctx.Rip += 0x3;
			}
		});
	}
}