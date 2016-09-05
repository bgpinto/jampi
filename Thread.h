#ifndef _THREAD_
#define _THREAD_

#include "Task.h"

namespace parallel {

class ThreadInterface {
public:
	virtual ~ThreadInterface(){}
	virtual void join() = 0;
};

template<class TASK>
class Thread : public virtual ThreadInterface {
public:
	virtual void operator()(TASK&) = 0;
	virtual ~Thread(){}

};
	

};

#endif
