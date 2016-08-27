#ifndef _THREAD_
#define _THREAD_

#include "Task.h"

// por enquanto, nao eh imposto relacionamento de copia entre threads;
// Obriga o usuario a sobrescrever o metodo [interface com a politica]	

namespace parallel {

/* TODO: Acho que thread nao precisa ser template */
template<typename Callable>
class Thread {

	Task<Callable>& current_task;

	public:
		Thread(const Task<Callable>& t):current_task(t) {}
		
		virtual void operator()() = 0;
		virtual void join() = 0;
		virtual ~Thread(){}

};
	

};

#endif
