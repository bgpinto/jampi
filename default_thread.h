#ifndef DEFAULT_THREAD
#define DEFAULT_THREAD

#include "Thread.h"

template<class TASK>
class DefaultThread : public virtual parallel::Thread<TASK> { 
	std::thread cpu;
public:
	DefaultThread() = default;
	~DefaultThread() = default;
	DefaultThread(const DefaultThread& t) = delete;
	DefaultThread& operator = (const DefaultThread& t) = delete;

	DefaultThread(DefaultThread&& t): cpu(std::move(t.cpu)) { }
	
	DefaultThread& operator = ( DefaultThread&& t) { 
		cpu = std::move(t.cpu);
		return *this;
	}		


	typename TASK::returnType_  operator()( TASK & t) { 
	
		auto task_future = t.getTaskFuture();
	
		cpu = std::thread(std::ref(t)); 
		//cpu = std::thread(t); 

		return task_future.get();
	}
	void join() {   if ( cpu.joinable()) cpu.join();   }

}; 

#endif
