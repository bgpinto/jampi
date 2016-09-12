#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

//#include <unordered_map>
#include <map> // pode nao ser problema com o unordered map
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

	std::multimap<int, parallel::ThreadInterface* > context_threads;
	
	using IndexIterator = typename std::multimap< int, parallel::ThreadInterface* >::iterator;
	
public:

	template<class TASK,  class U = void >	
	void spawn(TASK& t, int context = 0) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		T* thr = new T;

		std::multimap<int, parallel::ThreadInterface*>::iterator pp = context_threads.insert(std::pair<int, parallel::ThreadInterface* >(context, thr ));
		//context_threads.insert(std::pair<int, parallel::ThreadInterface* >(context, thr ));
		//if (pp->second == thr) std::cout << "Apontam iguais\n";

		thr->operator()(t);
		
		thr = NULL;
			

	}

	void sync(int context = 0) {

		std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);

		for (IndexIterator itr = joinable_threads.first; itr != joinable_threads.second; itr++) {
			
			if (itr->second == NULL) {  
				std::cout << "Found Null\n";
				
				break;
			}
			else {
				
				try {
					itr->second->join();
					delete itr->second;
				
				} catch(std::exception& e ) { 
					std::cout << "Exception no Join = "<< e.what() << std::endl;
				}	
			}

			/*	
			if (*itr->second != NULL) { 
				itr->second->join();
				delete itr->second;
			} else {
				std::cout << "DELETED POINTER !!\n";       
			}*/	
		
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
