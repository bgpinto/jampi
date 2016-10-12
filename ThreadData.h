#ifndef PARALLEL_THREADDATA__
#define PARALLEL_THREADDATA__

#include "Thread.h"

// debug mode

#include <iostream>


namespace parallel { 

class ThreadData{
	int context;	
	parallel::ThreadInterface* ptr;
public:
	
	ThreadData():context(0), ptr(nullptr){ }
	~ThreadData() { 
		if ( ptr != nullptr ) {  delete ptr;   }
	}

	void setThreadPointer(parallel::ThreadInterface* p) {  this->ptr = p; }
	
	void setThreadContext(int ctx) {  context = ctx; }

	parallel::ThreadInterface* getThreadPointer() { return ptr; }
	int getThreadContext() { return context; }	

	ThreadData(int ctx, parallel::ThreadInterface* p):context(ctx), ptr(p) { }

	bool equals(int ctx) { return context == ctx; }

	void join_request() {  ptr->join();  }
}; 

};
#endif
