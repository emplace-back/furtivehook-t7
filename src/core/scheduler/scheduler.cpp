#include "dependencies/std_include.hpp"
#include "scheduler.hpp"

namespace scheduler
{
	class task_pipeline
	{
	public:
		void add(task&& task)
		{
			new_callbacks_.access([&task](task_list& tasks)
			{
				tasks.emplace_back(std::move(task));
			});
		}

		void execute()
		{
			callbacks_.access([&](task_list& tasks)
			{
				this->merge_callbacks();

				for (auto i = tasks.begin(); i != tasks.end();)
				{
					const auto now = std::chrono::high_resolution_clock::now();
					const auto diff = now - i->last_call;

					if (diff < i->interval)
					{
						++i;
						continue;
					}

					i->last_call = now;

					const auto res = i->handler();

					if (res == cond_end)
					{
						i = tasks.erase(i);
					}
					else
					{
						++i;
					}
				}
			});
		}

	private:
		utils::concurrency::container<task_list> new_callbacks_;
		utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;

		void merge_callbacks()
		{
			callbacks_.access([&](task_list& tasks)
			{
				new_callbacks_.access([&](task_list& new_tasks)
				{
					tasks.insert(tasks.end(), std::move_iterator<task_list::iterator>(new_tasks.begin()), std::move_iterator<task_list::iterator>(new_tasks.end()));
					new_tasks = {};
				});
			});
		}
	};

	task_pipeline pipelines[pipeline::count];
	utils::hook::detour r_end_frame_hook;

	BOOL __stdcall query_performance_counter(LARGE_INTEGER* performance_count)
	{
		const auto ret_address{ reinterpret_cast<uintptr_t>(_ReturnAddress()) };

		if (ret_address == game::base_address + 0x20F88A9) // Com_Frame_Try_Block_Function
		{
			scheduler::execute(scheduler::pipeline::main);
		}
		
		return QueryPerformanceCounter(performance_count);
	}

	void r_end_frame_stub()
	{
		scheduler::execute(scheduler::pipeline::renderer); 
		return r_end_frame_hook.call<void>();
	}

	void execute(const pipeline type)
	{
		assert(type >= 0 && type < pipeline::count);
		pipelines[type].execute();
	}

	void schedule(const std::function<bool()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		assert(type >= 0 && type < pipeline::count);

		task task;
		task.handler = callback;
		task.interval = delay;
		task.last_call = std::chrono::high_resolution_clock::now();

		pipelines[type].add(std::move(task));
	}

	void loop(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_continue;
		}, type, delay);
	}

	void once(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_end;
		}, type, delay);
	}

	void on_dw_initialized(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([=]()
		{
			const auto dw_init = game::Live_IsUserSignedInToDemonware(0);

			if (dw_init)
			{
				once(callback, type, delay);
				return cond_end;
			}

			return cond_continue;
		}, type);
	}
	
	void initialize()
	{
		utils::hook::iat("kernel32.dll", "QueryPerformanceCounter", query_performance_counter);

		scheduler::once([]()
		{
			r_end_frame_hook.create(game::base_address + 0x1CD9E90, r_end_frame_stub);
		}, scheduler::pipeline::main);
	}
}