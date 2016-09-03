/*	TODO:
 *
 *	1. Melhorar os nomes dos parametros das templates;
 *
 *
 * */

#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <map>
#include <string>
#include "Thread.h"


/*
 * Aqui começa o problema. Uma politica pode criar varias threads, logo essas 
 * threads devem ser organizadas em um pool. O usuario nao gerencia o pool,
 * mas deve especificar qual pool utilizar.
 *
 *  * Vantagens:
 *  	- O usuario controla como as threads sao criadas/armazenadas
 *
 *  * Desvantagens:
 *  	- Syntaxe
 *  	- Precisa passar um ponteiro para funcao membro,
 *  	para que esta classe seja totalmente livre da interface
 *  	da classe pool
 *  	- O item acima atrapalha como serao sincronizadas as threads;
 *
 *
 *
 * */


namespace parallel {
// Aqui cabe SFINAE
// deveria eu criar esta classe como um singleton?
template
<
	class TASK,
	template <typename> class T,
	class Pool
>
class SchedulingPolicy {
	
	Pool workers;

	std::multimap<std::string, T<TASK>*> context_threads;
	
	using IndexIterator = typename std::multimap< std::string, T<TASK>* >::iterator;


	/*
	 *	A ideia do contexto funciona, porem:
	 *
	 *	* Uma funcao com os parametros template deve ser usada
	 *	para criar e executar a thread, nao a classe como um todo,
	 *	pois isso causa uma inflexibilidade no uso do escalonador
	 *	fazendo com que o escalonador soh possa funcionar para 
	 *	um determinado tipo de tarefa/thread e
	 *	torna dificil o uso da classe 
	 *
	 *
	 *
	 * */


	public:


		/*
		 *	Esse metodo deve ser repensado, pois:
		 *
		 *	Ao utilizar o conjunto de threads padrao da linguagem notou-se que,
		 *	como as threads sao criadas no escopo desta funcao, elas acabam
		 *	sendo destruidas antes de terem a oportunidade de sofrer join,
		 *	o que eh um erro. Logo, as threads devem ser alocadas dinamica-
		 *	mente. Nao sei ateh quando isto pode ser um problema.
		 *
		 *	Outro ponto eh que, para garantir a semantica de join em contexto,
		 *	precisei utilizar um map. Como posso expandilo de modo ah usa-lo
		 *	em escalonamento?? se nao puder usa-lo, como farei para que 
		 *	isso seja eficiente?? como dar espaço para o usuario implementar
		 *	o seu proprio algoritmo de escalonamento?
		 *
		 *
		 *
		 * */	
		void operator()(TASK& t, std::string& context) {
			//T<TASK> thr();
			//(workers.*ptr)(thr);
			//thr(t);
			
			//context_threads[context] = T<TASK>();



			//T<TASK> thr;
			
			//T<TASK>& rthr = thr;

			IndexIterator thr_ptr = context_threads.insert(std::pair<std::string, T<TASK>* >(context, new T<TASK> ));
			//thr(t);
			
			thr_ptr->second->operator()(t);

			//std::cout << thr_ptr->first << std::endl;

		}

		void sync(std::string& context) {

			//using IndexIterator = typename std::multimap< std::string, T<TASK>& >::iterator;

			std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);
		

			std::cout <<  "Joining from context === " << context  << std::endl;

			for (IndexIterator itr = joinable_threads.first; itr != joinable_threads.second; itr++) {
				itr->second->join();
				delete itr->second;	
			
			}
		

			//for (auto itr = joinable_threads.first; itr != joinable_threads.second; itr++ ) itr->second.join();
			
			context_threads.erase(context);	
			
			//std::cout <<  context_threads.size()  << std::endl;
			// isso deve mudar, pensar em uma forma melhor
			//for (std::size_t i = 0; i < workers.size(); i++) {
			//	workers[i].join();
			//}
		}
		
		~SchedulingPolicy(){}
		
		SchedulingPolicy():context_threads(){}
		SchedulingPolicy(const SchedulingPolicy&) = delete;
		SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
