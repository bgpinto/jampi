#ifndef _THREAD_
#define _THREAD_

#include "Task.h"

namespace parallel {

class ThreadInterface {
public:
	virtual ~ThreadInterface() {} 
	virtual void join() = 0;
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
template<class TASK>
class Thread : public virtual ThreadInterface {
public:
	virtual void operator()(TASK&) = 0;
	virtual ~Thread(){} 

};
	

};

#endif
