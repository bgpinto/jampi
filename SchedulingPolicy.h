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
#include <unordered_map>


#include "ThreadData.h"

#include "SchedAlgorithm.h"

namespace parallel {

template
<
	class Algorithm,
	class THREAD = void, 
	class AssertCompatible = typename std::enable_if< std::is_base_of< typename parallel::SchedAlgorithm, Algorithm>::value >::type
>
class SchedulingPolicy {
	
	Algorithm scheme;
	std::mutex thread_table_mutex;
	std::atomic<int> context_allocate;
	int thread_counter;

	//parallel::ThreadData* thread_table;

	std::unordered_multimap<int, parallel::ThreadInterface*> thread_table;
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
		
		context_allocate.store(context);

		// ah ideia aqui eh: associa um vp com esse contexto e tarefa
		//T* thr = scheme.template create<T>(context);
		T* thr = new T;

		// salva esse vp + contexto + task 
		//thread_table[thread_counter].setThreadPointer(thr);
		//thread_table[thread_counter].setThreadContext(context);
		
		thread_table.insert(std::make_pair(context, thr));

		thread_counter++;

		context_allocate.store(-1);
		
		thread_table_mutex.unlock();

		// sera que bloqueia?
		// roda essa task nesse vp e retorna seu resultado
		return thr->operator()(t);

		
		// Acho que a chamada para execute vai precisar saber
		// o tipo de thread tambem
		//return scheme.template execute<TASK>(t, context );

		//return t.getTaskReturn();		
	
	}

	// ah ideia aqui eh:
	// pede para aquele vp que tem a tarefa associada com o contexto x terminar de executar
	// a tarefa
	//
	
	void sync(int context = 0) { 
	
		// nao sincroniza enquanto aloca	
		int current_context = context_allocate.load();
		while(!context_allocate.compare_exchange_weak(current_context, context) );

		auto joinable_threads = thread_table.equal_range(context);

		for (auto it = joinable_threads.first; it != joinable_threads.second; ++it ) { 
			try { 
				it->second->join();
				//thread_table.erase(it);
				//delete it->second;
			} catch(std::system_error& se) { 
			
			}
		}


		// deveria chamar erase, mas acho que da problema
	}


	/*
	void sync(int context = 0) {
		int inserted_threads = thread_counter; 
		for (int i = 0; i < inserted_threads; i++) { 
			if ( thread_table[i].equals(context)) { 
				try { 
					int current_context = context_allocate.load();
					while(!context_allocate.compare_exchange_weak(current_context, context) );
						
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
	}*/
	
	
	
	// Talvez eu tenha que guardar size para o caso do cara
	// tentar alocar mais threads que size;;
	/*
	SchedulingPolicy(const unsigned int size): 
			 context_allocate(0),
			 thread_counter(0)
			{ thread_table = new parallel::ThreadData[size];}

	*/

	SchedulingPolicy() = default;
	
	~SchedulingPolicy(){

		for (auto& it : thread_table ) {    
			try { 
				
				if (it.second != nullptr) { 
					it.second->join();
					delete it.second;
				}

			}catch (std::system_error& se ) { 
			
			}

		}

				
	}
	
	
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

	SchedulingPolicy(SchedulingPolicy&&) = delete;
	SchedulingPolicy& operator = (SchedulingPolicy&&) = delete;

};


};

#endif
