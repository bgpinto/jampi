/*
 *	NOTE:
 *
 *	Este codigo nao compila, pois ha ainda coisas
 *	que precisam ser resolvidas na classe Task.
 *	No entanto, eh o esqueleto a implementacao final
 *	do padrao for paralelo;
 *
 *	Este esboco eh importante, pois expoe servicos
 *	necessarios que nao estao presentes na classe
 *	policy, por exemplo.
 *
 * */

#ifndef _REDUCE_
#define _REDUCE_

#include <type_traits>
#include "Policy.h"

template
<
	typename T,
	typename ReductionOperator, 
	typename <typename> class P = Policy<Thread>,
	class AssertCompatible =
	std::enable_if<
		std::is_base_of< parallel::Policy, P>::value
	>::type
>
T reduce(T& collection, int size) { // preciso passar o tamanho?

	ReductionOperator op;
	
	// versao iterativa do algoritmo	


	return T();
}
#endif
