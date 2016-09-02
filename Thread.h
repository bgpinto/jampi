#ifndef _THREAD_
#define _THREAD_

#include "Task.h"

namespace parallel {

template<class TASK>
class Thread {
	public:
		virtual void operator()(TASK&) = 0;
		virtual void join() = 0;
		virtual ~Thread(){}

};
	

};

#endif
