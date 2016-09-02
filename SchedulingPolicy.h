/*	TODO:
 *
 *	1. Melhorar os nomes dos parametros das templates;
 *
 *
 * */

#ifndef _SCHEDULINGPOLICY_
#define _SCHEDULINGPOLICY_

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
template
<
	class TASK,
	template <typename> class T,
	class Pool
>
class SchedulingPolicy {
	
	Pool workers;

	public:
	
		virtual void operator()(TASK& t) {
			//T<TASK> thr();
			//(workers.*ptr)(thr);
			//thr(t);	
		}

		virtual void sync() {
			
			// isso deve mudar, pensar em uma forma melhor
			//for (std::size_t i = 0; i < workers.size(); i++) {
			//	workers[i].join();
			//}
		}
		virtual ~SchedulingPolicy(){}
		
		SchedulingPolicy(){}
		SchedulingPolicy(const SchedulingPolicy&) = delete;
		SchedulingPolicy& operator = (const SchedulingPolicy&) = delete;

};


};

#endif
