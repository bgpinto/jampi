#ifndef DEFAULT_ALGORITHM
#define DEFAULT_ALGORITHM


#include "SchedAlgorithm.h"


namespace parallel { 

class DefaultAlgorithm : parallel::SchedAlgorithm { 
	parallel::ThreadInterface* last_created_thread; 
public: 
	DefaultAlgorithm():last_created_thread(nullptr) {}

	template<class THREAD>
	THREAD* create_thread(int ctx) { 
		THREAD* thr = new THREAD; 
		last_created_thread = thr; 
		return thr;	
	} 

	template<class TASK>
	void execute(TASK& t, int ctx ) { 
		parallel::Thread<TASK>* curr_thread = dynamic_cast<parallel::Thread<TASK>*>(last_created_thread); 
		curr_thread->operator()(t); 
	} 
};

};

#endif
