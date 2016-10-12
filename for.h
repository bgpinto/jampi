
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
#include "Task.h"
#include "SchedulingPolicy.h"

#include "Range.h"
#include "ContextGenerator.h"


namespace parallel {

template
<
	class SCHED,
	template <typename> class ThreadType,
	typename Func

>
void for_helper(SCHED& policy , parallel::Range& r, int chunk, Func& f ) { 

	int size = r.size(); 

	if ( (size/chunk) > 1 ) { 
	
		int shift = size / 2;

		parallel::Range left_range( r.begin(), r.begin() + shift );
		parallel::Range right_range( r.begin() + shift, r.end() );

		parallel::Task<void, SCHED&, parallel::Range& , int, Func&> left(for_helper<SCHED, ThreadType, Func>, policy, left_range, chunk, f );
		parallel::Task<void, SCHED&, parallel::Range& , int, Func&> right(for_helper<SCHED, ThreadType, Func>, policy, right_range, chunk, f );


		int context = generate_context(shift * 7 * 29);
	       
		using TASK = decltype(left);

		policy.template spawn< TASK, ThreadType<TASK> >(left, context);
		policy.template spawn< TASK, ThreadType<TASK> >(right, context);

		policy.template sync(context);	


	} else f(r); 
	
	 
}


template
<
	typename SchedAlg,
       	template <typename> class ThreadType,
	typename Func	
>
void For(parallel::Range& r, int chunk, Func& f ) { 

	if (chunk == 0) return;

	parallel::SchedulingPolicy<SchedAlg> scheduler(10000);

	for_helper<decltype(scheduler), ThreadType, Func>(scheduler, r, chunk, f );


}




};


#endif
