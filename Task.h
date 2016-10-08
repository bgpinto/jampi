#ifndef _TASK_
#define _TASK_

#include <memory>
#include <tuple>
#include <utility>
#include "Callable.h"

/*
 *	Hipotese: parece que make_integer_sequence nao funciona/existe no g++ 4.8.4
 *	Falso, existe, porem sobre a flag -std=c++14
 * */

namespace parallel {

/*
 *  Uma feature potencialmente util para esta classe seria a capacidade
 *  de se converter/retornar no callable que a mesma 'gerencia' 
 *  ( parece nao ser algo tao simples de fazer.) 
 * */

/*
 *	Como esta classe suporta funcoes template? (membro ou nao)
 *	Funciona perfeitamente. Ponto pra mim.
 *
 * */


template
<
	typename ReturnType = void,
	typename ...Args
>
class Task {
	
	std::unique_ptr< Callable<ReturnType, Args...> > function;
	
	std::tuple<Args...> arguments;

	template<std::size_t... index>
	ReturnType unpackArguments(std::index_sequence<index...>) {
		if (function) return (*function)(std::get<index>(arguments)...);

		return ReturnType();
	}

	public:
		
		Task() = default;
		~Task(){}
		
		// O objeto copiado eh destruido	
		
		// Aparentemente, esta versao eh a forma correta de copiar os ptrs
		// deveria modificar no resto;	-- Errado, com get nao ha copia profunda
		// Precisa do -fpermissive	
		/*
		Task(const Task& t):arguments(t.arguments) {
		       function.reset(t.function.get());
		       //t.function.reset();
		 		
		}*/

		Task& operator = (const Task& t) {
			function.reset(t.function.release());
		       	arguments = t.arguments;	
			return *this;
		}

		Task(const Task& t):arguments(t.arguments) {
		       function.reset(t.function.release());
		}

		Task& operator = (Task& t) {
			function.reset(t.function.release());
		       	arguments = t.arguments;	
			return *this;
		}

		// Callable tricks for non-member function calls
		template<class Fn>
		Task(Fn f, Args...args);

		template<class PtrToMember, class PtrToObject>
		Task(PtrToMember m, PtrToObject o, Args...args);

		ReturnType operator () () {
			constexpr auto size = std::tuple_size<decltype(arguments)>::value;
			return unpackArguments(std::make_index_sequence<size>{});
		}

};

template
<
	typename ReturnType,
	typename ...Args

>
template<class Fn>
Task<ReturnType, Args...>::Task(Fn f, Args...args):function( new CallableImpl<Fn, ReturnType, Args...>(f) ),arguments(std::tie(args...))
{}


template
<
	typename ReturnType,
	typename ...Args
>
template<class PtrToMember, class PtrToObject>
Task<ReturnType, Args...>::Task(PtrToMember m, PtrToObject o, Args...args):function( new MemberCallableImpl<PtrToMember, PtrToObject, ReturnType, Args...>(m, o) ), arguments(std::tie(args...))
{}


}
#endif
