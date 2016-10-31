#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <mutex>
#include <atomic>
#include <type_traits>
#include <system_error>
#include <future>
#include <unordered_map> 

#include "SchedAlgorithm.h"

namespace parallel {

template
<
	class Algorithm,
	class THREAD = void, 
	class AssertCompatible = typename std::enable_if< std::is_base_of< typename parallel::SchedAlgorithm, Algorithm>::value >::type
>
class SchedulingPolicy {
	
	Algorithm scheme;
	std::mutex thread_table_mutex;
	std::atomic<int> context_allocate;
	std::unordered_multimap<int, parallel::ThreadInterface*> thread_table;
public:

	template<class TASK,  class U = void >	
	typename TASK::returnType_ spawn(TASK& t, int context = 0) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		thread_table_mutex.lock();
		
		context_allocate.store(context);

		T* thr = scheme.template create_thread<T>(context);

		if(thr) thread_table.insert(std::make_pair(context, thr));

		context_allocate.store(-1);
		
		thread_table_mutex.unlock();

		auto task_result = t.getTaskFuture();

		scheme.template execute<TASK>(t, context);

		return 	task_result.get();
	}
	
	void sync(int context = 0) { 
	
		int current_context = context_allocate.load();
		while(!context_allocate.compare_exchange_weak(current_context, context) );

		auto joinable_threads = thread_table.equal_range(context);

		for (auto it = joinable_threads.first; it != joinable_threads.second; ++it ) { 
			try { 
				it->second->join();
			} catch(std::system_error& se) { 
			
			}
		}


	} 

	
	~SchedulingPolicy(){

		for (auto& it : thread_table ) {    
			try { 
				
				if (it.second != nullptr) { 
					it.second->join();
					delete it.second;
				}

			}catch (std::system_error& se ) { 
			
			}

		}

				
	}
	
	
	SchedulingPolicy() = default;
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;
	SchedulingPolicy(SchedulingPolicy&&) = delete;
	SchedulingPolicy& operator = (SchedulingPolicy&&) = delete;

};


};

#endif
