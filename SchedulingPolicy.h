/*	TODO:
 *
 *	1. Melhorar os nomes dos parametros das templates;
 *
 *
 * */

#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

#include <unordered_map>
#include <type_traits>
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
// deveria eu criar esta classe como um singleton?
// Aqui acho que poderia parametrizar com o conteiner e o algoritmo de escalonamento
// Contexto pode ser: const char* (ver func_macro.cpp)
//
// Devo usar unordered_map por ser mais eficiente para acesso e agrupamento de threads;

template
<
	class Algorithm,
	class THREAD = void 
>
class SchedulingPolicy {
	
	Algorithm scheme;

	std::unordered_multimap<std::string, parallel::ThreadInterface* > context_threads;
	
	using IndexIterator = typename std::unordered_multimap< std::string, parallel::ThreadInterface* >::iterator;
	
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
	template<class TASK,  class U = void >	
	void spawn(TASK& t, std::string& context) {

		typedef typename std::conditional<std::is_same<void, THREAD>::value, U, THREAD >::type T;

		T* thr = new T;

		context_threads.insert(std::pair<std::string, parallel::ThreadInterface* >(context, thr ));
		
		thr->operator()(t);	

	}

	void sync(std::string& context) {

		std::pair<IndexIterator, IndexIterator> joinable_threads = context_threads.equal_range(context);

		for (IndexIterator itr = joinable_threads.first; itr != joinable_threads.second; itr++) {
			itr->second->join();
			delete itr->second;	
		
		}
		
		context_threads.erase(context);	
		
	}
	
	~SchedulingPolicy(){}
	
	SchedulingPolicy():context_threads(){}
	SchedulingPolicy(const SchedulingPolicy&) = delete;
	SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
