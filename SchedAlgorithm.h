#ifndef PARALLEL_SCHEDALGORITHM
#define PARALLEL_SCHEDALGORITHM

#include "Thread.h"


namespace parallel {

class SchedAlgorithm {  
public:
	template<class THREAD>
	THREAD* create_thread(int ctx) { return nullptr; }
	
	template<class TASK>
	void execute(TASK& t, int ctx) { }

	virtual ~SchedAlgorithm() = default;
};



};


#endif
