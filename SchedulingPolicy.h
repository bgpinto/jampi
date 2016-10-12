#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <unordered_map>
//#include <map> // pode nao ser problema com o unordered map
#include <algorithm>
#include <type_traits>
#include <exception>
#include <system_error>
#include <thread>

#include <mutex>


#include "Thread.h"

std::mutex context_mutex;


namespace parallel {

template
<
	class Algorithm,
	class THREAD = void 
>
class SchedulingPolicy {
	
	Algorithm scheme;

	std::unordered_multimap<int, parallel::ThreadInterface* > context_threads;
	
	using IndexIterator = typename std::unordered_multimap< int, parallel::ThreadInterface* >::iterator;
	
public:

	template<class TASK,  class U = void >	
	void spawn(TASK& t, int context = 0) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		T* thr = new T;
		
		if(thr != nullptr) {
			context_threads.emplace(context, thr);
			thr->operator()(t);
		} 
	}


	// o que acontece se duas threads querem dar join em uma mesma thread ou acessarem 
	// a tabela;
	void sync(int context = 0) {

		std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);

		/*
		context_mutex.lock();
		for (IndexIterator it = joinable_threads.first; it != joinable_threads.second; it++) { 	
			std::cout << it->first << " -- " << it->second << std::endl;
		}
		context_mutex.unlock();		
		
		*/
		for (IndexIterator it = joinable_threads.first; it != joinable_threads.second; it++) { 

				try { 
					if (it->second != nullptr) { 
						it->second->join();
					}
				} catch (std::system_error& e ) {
				       
					if ( e.code() ==  std::errc::invalid_argument || e.code() == std::errc::no_such_process) { 
						std::cout << "Foooo\n";
					}
						
				       	
				
				}
			}	
		//context_mutex.unlock();

	}
	
	~SchedulingPolicy(){}
	
	SchedulingPolicy():context_threads(){}
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
