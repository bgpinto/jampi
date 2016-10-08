#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <unordered_map>
//#include <map> // pode nao ser problema com o unordered map
#include <algorithm>
#include <type_traits>
#include <exception>


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
	void spawn(TASK& t, int context = 0) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		T* thr = new T;
		
		//T* thr = scheme.create<T>( task, context);

		//std::multimap<int, parallel::ThreadInterface*>::iterator pp = context_threads.insert(std::pair<int, parallel::ThreadInterface* >(context, thr ));
		context_threads.insert(std::pair<int, parallel::ThreadInterface* >(context, thr ));
		//if (pp->second == thr) std::cout << "Apontam iguais\n";

		thr->operator()(t);
		
		//thr = NULL;
			

	}

	void sync(int context = 0) {

		//std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);

		//std::cout << "Joining Context = " << context << std::endl;

		auto joinable_threads = context_threads.equal_range(context);

		for_each(joinable_threads.first, joinable_threads.second, [](std::unordered_multimap<int, parallel::ThreadInterface*>::value_type& thread){
			
			thread.second->join();		
		});
	
		/*
		for (IndexIterator itr = joinable_threads.first; itr!= joinable_threads.second; ++itr){
			
			if (itr->second == nullptr) {  
				std::cout << "Found NULLLL\n";	
			}
			else {
				try {
						if(itr->second != nullptr) {  
							itr->second->join(); // ainda da segfault
							delete itr->second;
						}
					} catch(std::exception& e ) { 
						std::cout << "Exception no Join = "<< e.what() << std::endl;
						
						if (!itr->second) delete itr->second;
					}	
				}

		
		}
		
		//context_threads.erase(context);	
		*/
	}
	
	~SchedulingPolicy(){}
	
	SchedulingPolicy():context_threads(){}
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
