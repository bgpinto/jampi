
/*	
 *	TODO:
 *
 *	1. Melhorar nomes de identificadores
 *	2. Implementar compatibilidade da range com std::begin/end
 *	3. Implementar uma funcao auxiliar para gerar essa range
 *	4. Adicionar PARALLEL a todas as macros guarda
 *	5. Adicionar SIFNAE se possivel
 * */

#ifndef _FOR_
#define _FOR_

#include <type_traits>
#include <string>
#include "Task.h"
#include "SchedulingPolicy.h"

#include <iostream>

namespace parallel {


template
<
	class S,
	template <typename> class T,
	typename R,
	typename Fn
        
>
void split_and_run(S& policy, R& r, Fn& f, int grainSize) {

	// posteriormente, revisar este algoritmo de divisao	

	std::cout << "From Split and Run with rb = " << r.begin() << " re = " << r.end() << std::endl;

	int size = r.end() - r.begin();
	


	if (((double)size/grainSize) > 1.0 ) {
	
		std::cout << "Division factor = " << (double)size/grainSize <<std::endl; 

		// substituir por uma funcao e std;;begin/end
		int alpha = size / 2;
		R right(r.begin(), r.begin() + alpha);
		R left(r.begin() + alpha, r.end());

		parallel::Task<void, S&, R&, Fn&, unsigned int>
		       	for_job_right(split_and_run<S,T, R,Fn>, policy, right, f, grainSize);	
		
		parallel::Task<void, S&, R&, Fn&, unsigned int>
		       	for_job_left(split_and_run<S,T, R,Fn>, policy, left, f, grainSize);	

		using TASK = decltype(for_job_right);
		
		// tenho que achar um jeito de gerar um contexto unico 
		// para cada grupo de thread por chamada de funcao
		// se nao, da problema
		int context = r.begin() + alpha;	
		
		
		std::cout << "Context is = " << context << std::endl;

		// push(for_job_left)		

		policy.template spawn<TASK, T<TASK> >(for_job_right, context);
		
		// if j == pop : policy.spawn... j else alguem robou j da lista
		policy.template spawn<TASK, T<TASK> >(for_job_left, context);
		
		policy.sync(context);
	
	} else {
	
		f(r);
	}

}

template
<	
	typename SchedAlg,
	template <typename>class ThreadType,
	typename R,
	typename Fn
>
void for_(R& r, Fn& f, int grainSize) { // o callable acho q deve ser copiado
// mudar o nome do parallel_for

	// substituir por uma funcao e std;;begin/end
	int half = r.end() / 2;
	R right(r.begin(), half); // pode ser que os problemas sejam por causa disso:
					// passando range criada na pilha
	R left(half, r.end());

	std::cout << "Right begin and end = " << right.begin() << " " << right.end() << std::endl;
	std::cout << "Left begin and end = " << left.begin() << " " << left.end() << std::endl;

	parallel::SchedulingPolicy<SchedAlg> scheduler;
	
	using SCHED = decltype(scheduler);
	
	parallel::Task<void, SCHED&, R&, Fn&, unsigned int> 
		for_job_right(split_and_run<SCHED, ThreadType, R, Fn>, scheduler, right, f, grainSize);	
	parallel::Task<void, SCHED&, R&, Fn&, unsigned int> 
		for_job_left(split_and_run<SCHED, ThreadType, R, Fn>, scheduler, left, f, grainSize);

	using TASK = decltype(for_job_left);

	int context = 0;	

	scheduler.template spawn<TASK, ThreadType<TASK>>(for_job_right, context);
	scheduler.template spawn<TASK, ThreadType<TASK>>(for_job_left, context);

	scheduler.sync(context);	

}


};


#endif
