
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

#include "Range.h"

#include <iostream>
#include <cstdio>

namespace parallel {


template
<
	class S,
	template <typename> class T,
	typename Fn
        
>
void split_and_run(S& policy, Range& r, Fn& f, int grainSize, int last_context) {

	// posteriormente, revisar este algoritmo de divisao	

	//std::cout << "From Split and Run with rb = " << r.begin() << " re = " << r.end() << std::endl;

	int size = r.end() - r.begin();
	


	if (( size/grainSize) > 1.0 ) {
	
		//std::cout << "Division factor = " << (double)size/grainSize <<std::endl; 

		// substituir por uma funcao e std;;begin/end
		int alpha = size / 2;
		Range right(r.begin(), r.begin() + alpha);
		Range left(r.begin() + alpha, r.end());


		int context_left  = last_context * 2;
		int context_right = (last_context * 2) + 1;

		// nota interessante: overhead de criacao de tarefas?
		parallel::Task<void, S&, Range&, Fn&, int, int>
		for_job_right(split_and_run<S, T, Fn>, policy, right, f, grainSize, context_right);	
		
		parallel::Task<void, S&, Range&, Fn&, int, int>
		for_job_left(split_and_run<S, T, Fn>, policy, left, f, grainSize, context_left);	

		using TASK = decltype(for_job_right);
		
		// tenho que achar um jeito de gerar um contexto unico 
		// para cada grupo de thread por chamada de funcao
		// se nao, da problema
		//int context = r.begin() + alpha;	
		
		//printf("Context is %d\n", context);

		//std::cout << "Context is = " << context << std::endl;

		// push(for_job_left)		

		policy.template spawn<TASK, T<TASK> >(for_job_right, context_right);
		
		// if j == pop : policy.spawn... j else alguem robou j da lista
		policy.template spawn<TASK, T<TASK> >(for_job_left, context_left);
		
		policy.sync(context_right);
		policy.sync(context_left);
	
	} else {
	
		f(r);
	}

}

template
<	
	typename SchedAlg,
	template <typename>class ThreadType,
	typename Fn
>
void For(Range& r, int grainSize, Fn& f)  { 

	// substituir por uma funcao e std;;begin/end
	int half = r.end() / 2;
	Range right(r.begin(), half);
	Range left(half, r.end());

	std::cout << "Right begin and end = " << right.begin() << " " << right.end() << std::endl;
	std::cout << "Left begin and end = " << left.begin() << " " << left.end() << std::endl;

	parallel::SchedulingPolicy<SchedAlg> scheduler;
	
	using SCHED = decltype(scheduler);
	
	parallel::Task<void, SCHED&, Range&, Fn&, int, int> 
	for_job_right(split_and_run<SCHED, ThreadType,  Fn>, scheduler, right, f, grainSize, 1);

	parallel::Task<void, SCHED&, Range&, Fn&, int, int> 
	for_job_left(split_and_run<SCHED, ThreadType,  Fn>, scheduler, left, f, grainSize, 2);

	using TASK = decltype(for_job_left);

	//int context = 0;	
	
	scheduler.template spawn<TASK, ThreadType<TASK>>(for_job_right, 1);
	scheduler.template spawn<TASK, ThreadType<TASK>>(for_job_left,  2);

	scheduler.sync(1);	
	scheduler.sync(2);	
}


};


#endif
