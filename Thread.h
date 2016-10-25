#ifndef _THREAD_
#define _THREAD_

#include <type_traits>
#include <future>

#include "Task.h"

namespace parallel {

class ThreadInterface {
public:
	virtual ~ThreadInterface() {} 
	virtual void join() = 0;
	
	ThreadInterface() = default;
	ThreadInterface(const ThreadInterface&) = default;
	ThreadInterface(ThreadInterface&&) = default;
	ThreadInterface& operator = (const ThreadInterface&) = default;
	ThreadInterface& operator = (ThreadInterface&&) = default;

	
};

// Uma ideia: tentar criar um type trait que deduz o tipo
// de retorno de task, o que implicaria em poder criar
// chamadas do tipo auto x = scheduler.spawn(...);
// isso melhoraria a interface e a tornaria muito
// proxima a cilk plus, com o benecificio de
// que construcoes recursivas ficariam mais claras
// possiveis impedimentos:
// (1) virtual nao pode ser template
// (2) como descobrir o returntype de task? nao eh dificil TASK::ReturnType
template< class TASK >
class Thread : public virtual ThreadInterface {
public:
	virtual typename TASK::returnType_ operator()(TASK&) = 0;
	virtual ~Thread(){}

	Thread() = default;	
	Thread(const Thread&) = default;	
	Thread(Thread&&) = default;	
	Thread& operator = (const Thread&) = default;	
	Thread& operator = (Thread&&) = default;	
};
	

};

#endif
