#ifndef PARALLEL_SCHEDALGORITHM
#define PARALLEL_SCHEDALGORITHM

#include "Task.h"
#include "Thread.h"


namespace parallel {

class SchedAlgorithm {  
public:
	// se o vp nao existe nesse contexto, criar, se nao retorna o ptr
	template<class THREAD>
	THREAD* create_thread(int ctx) { return nullptr; }
	
	// para o vp associado com esse contexto, executa essa tarefa
	template<class TASK>
	typename TASK::returnType_ execute(TASK& t, int ctx) { return TASK::returnType_(); }

};



};


#endif
