#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

/*
 * NOTE:
 *  (1)  Nessa implementacao, nao estou preocupado com a forma que a memoria da 
 *  tabela de threads eh gerenciada. Logo, threads que sofrem join nao estao 
 *  sendo deletadas (ainda, isso poderia ser implementado em ThreadData::join_request)
 *  
 *  (2) No metodo spawn:
 *  Se eu conseguir adicionar a informacao da template em T, o usuario nao precisa 
 *  digitar isso. Eh algo a se pensar;
 *
 * */


#include <mutex>
#include <atomic>
#include <type_traits>
#include <system_error>
#include <future>

#include "ThreadData.h"

#include "SchedAlgorithm.h"

namespace parallel {

template
<
	class Algorithm,
	class AssertCompatible = typename std::enable_if< std::is_base_of< typename parallel::SchedAlgorithm, Algorithm>::value >::type,
	class THREAD = void 
>
class SchedulingPolicy {
	
	Algorithm scheme;
	std::mutex thread_table_mutex;
	std::atomic<int> context_join;
	int thread_counter;

	parallel::ThreadData* thread_table;

public:

	// A proposta:
	// Irei introduzir a interface do algoritmo de escalonamento
	// logo, ela tem q ser generica a ponto de:
	// (1) nao alterar a semantica do spawn, mesmo mudando 2 linhas importantes
	// (2) nao alterar a semantica do join
	template<class TASK,  class U = void >	
	typename TASK::returnType_ spawn(TASK& t, int context = 0) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		thread_table_mutex.lock();
		
		context_join.store(context_join);

		// ah ideia aqui eh: associa um vp com esse contexto e tarefa
		T* thr = scheme.template create<T>(context);

		// salva esse vp + contexto + task 
		thread_table[thread_counter].setThreadPointer(thr);
		thread_table[thread_counter].setThreadContext(context);
		thread_counter++;

		context_join.store(-1);
		
		thread_table_mutex.unlock();

		// sera que bloqueia?
		// roda essa task nesse vp e retorna seu resultado
		//return thr->operator()(t);

		return scheme.template execute<TASK>(t, context );

		//return t.getTaskReturn();		
	
	}

	// ah ideia aqui eh:
	// pede para aquele vp que tem a tarefa associada com o contexto x terminar de executar
	// a tarefa
	void sync(int context = 0) {
		int inserted_threads = thread_counter; 
		for (int i = 0; i < inserted_threads; i++) { 
			if ( thread_table[i].equals(context)) { 
				try { 
					int current_context = context_join.load();
					while(!context_join.compare_exchange_weak(current_context, context) );
						
					thread_table[i].join_request();			

				} catch(std::system_error& se) { 
					#ifdef EXECPTION_ON
						std::cout <<
						"Exception fired at: method sync with context = " << context 
						<< std::endl; 
					#endif	
				}
			 }

		}
	}
	
	
	
	// Talvez eu tenha que guardar size para o caso do cara
	// tentar alocar mais threads que size;;
	SchedulingPolicy(const unsigned int size): 
			 context_join(0),
			 thread_counter(0)
			{ thread_table = new parallel::ThreadData[size];}

	~SchedulingPolicy(){

		for (int i = 0; i < thread_counter; i++) { 
			if (thread_table[i].getThreadPointer()) thread_table[i].join_request();
		}
		
		if (thread_table != nullptr) {  delete[] thread_table;  }
	}
	
	
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
