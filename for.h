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

#ifndef _FOR_
#define _FOR_

#include <type_traits>
#include "Policy.h"

namespace parallel {


template
<
	typename P = Policy<Thread>,
	typename R = Range,
	typename C = Callable,
        typename ...Args	
>
void split_and_run(P& policy, R& r, C fn, Args... args) {

	if (r.end() > r.begin()) {

		// Precisa dividir a range e passar para a nova tarefa

		// Gera uma tarefa para continuar o for em paralelo
		//parallel::Task<decltype(split_and_run)> job(/*... Argumentos e nao soh o codigo chamador*/);

		// Cria uma thread e associa a nova tarefa para carregar a execucao em paralelo seguindo a politica
		policy(job);
		
		// Executa a porcao do for ao qual a thread eh responsavel
		//fn(args...);
	
		policy.sync(); // sincroniza a execucao das outras threads

	}

}
	

template
<
	typename <typename> class P = Policy<Thread>,
	typename R = Range,
	typename C = Callable,
	typename ...Args,
	class AssertCompatible =
	std::enable_if<
		std::is_base_of< parallel::Policy, P>::value
	>::type
>
for(R& r, C fn, Args... args) { // o callable acho q deve ser copiado

	P policy;

	int half = r.end() / 2;
	R right(r.begin(), half);
	R left(half, r.end());

	parallel::Task<> for_job_right();	
	parallel::Task<> for_job_left();

	policy(for_job_left);
	policy(for_job_right);

	policy.sync_all();	

}


};


#endif
