#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <unordered_map>
#include <type_traits>
#include "Thread.h"



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
	void spawn(TASK& t, int context) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		T* thr = new T;

		context_threads.insert(std::pair<int, parallel::ThreadInterface* >(context, thr ));
		
		thr->operator()(t);	

	}

	void sync(int context) {

		std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);

		for (IndexIterator itr = joinable_threads.first; itr != joinable_threads.second; itr++) {
			itr->second->join();
			delete itr->second;	
		
		}
		
		context_threads.erase(context);	
		
	}
	
	~SchedulingPolicy(){}
	
	SchedulingPolicy():context_threads(){}
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
