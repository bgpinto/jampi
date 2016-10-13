#ifndef _TASK_
#define _TASK_

#include <memory>
#include <tuple>
#include <utility>

#include <future>

#include "Callable.h"


namespace parallel {

/*
 *  Uma feature potencialmente util para esta classe seria a capacidade
 *  de se converter/retornar no callable que a mesma 'gerencia' 
 *  ( parece nao ser algo tao simples de fazer.) 
 * */


/* NOTE: Thu 13 Oct 2016 11:26:59 AM BRT
 *	
 *	(1) Seria legal ter uma forma de passar os parametros do invocavel
 *	em um outro momento no tempo e nao apenas na construcao de uma tarefa.
 *
 *	Algo do tipo: set_args(Args ....args) {  tuple = args;  }
 *
 *
 *
 * */


template<int ...> struct seq {};

template<int N, int ...S> struct gens : gens<N-1, N-1, S...> {};

template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };


template
<
	typename ReturnType = void,
	typename ...Args
>
class Task {
	
	std::unique_ptr< Callable<ReturnType, Args...> > function;
	
	std::tuple<Args...> arguments;

	
	std::promise<ReturnType> return_channel;

	template<int ... index>
	ReturnType unpackArguments(seq<index ...>){
		if (function) return_channel.set_value( (*function)(std::get<index>(arguments)...) ) ;

		return ReturnType();
	}

	public:
		
		typedef ReturnType returnType_; 
		

		Task():function(nullptr), arguments(), return_channel() { }
		~Task(){}
		
		// O objeto copiado eh destruido	
		//Task(Task& t):arguments(t.arguments), function( new Callable<ReturnType, Args ...>( *t.function )) {
		       //t.function.reset();
		 		
		//}

		/*	
		Task& operator = (const Task& t) {
			function.reset(t.function.release());
		       	arguments = t.arguments;	
			return *this;
		}*/

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
			return unpackArguments(typename gens<sizeof...(Args)>::type());
		}

		std::future<ReturnType> getTaskFuture() { return return_channel.get_future(); }

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
