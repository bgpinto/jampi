#ifndef _TASK_
#define _TASK_

#include <memory>
#include <tuple>
#include <utility>

#include <future>

#include "Callable.h"

#include <iostream>


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


// Mesmo problema que com thread, porem
// uma possibilidade seria usar 
// getTaskFuture como funcao template, talvez resolva; [ verificar viabilidade]
class TaskInterface {
public:
	virtual void operator()() = 0;
	virtual ~TaskInterface(){}

	// adicionando depois dos testes com move semantics [ pode ser problema ]
	
	TaskInterface() = default;
	TaskInterface(const TaskInterface&) = default;
	TaskInterface& operator = (const TaskInterface&) = default;

	TaskInterface(TaskInterface&&) = default;
	TaskInterface& operator = (TaskInterface&&) = default;
		
};



template
<
	typename ReturnType = void,
	typename ...Args
>
class Task : public virtual TaskInterface {
	
	std::unique_ptr< Callable<ReturnType, Args...> > function;
	
	std::tuple<Args...> arguments;

	
	std::promise<ReturnType> return_channel;

	template<int ... index>
	void unpackArguments(seq<index ...>){
		if (function) return_channel.set_value( (*function)(std::get<index>(arguments)...) ) ;
	}

	public:
		
		typedef ReturnType returnType_; 
		
		Task():function(nullptr), arguments(), return_channel() { }
		
		Task(const Task& t):arguments(t.arguments) {
	      
			Task& other = const_cast<Task&>(t);
			function = std::move(other.function);
			return_channel = std::move(other.return_channel); 
		}
		
		Task& operator=(const Task& t) {
	       		
			Task& other = const_cast<Task&>(t); 
			arguments = std::move(t.arguments);
			function = std::move(other.function);
			return_channel = std::move(other.return_channel); 
			return *this; 
		}

	
		Task( Task&& other):
			function(std::move(other.function)),
			arguments( std::move(other.arguments)),
		     	return_channel( std::move(other.return_channel))	{ }

		Task& operator = (Task&& other) {
			
			function  = std::move(other.function);
			arguments = std::move(other.arguments);
			return_channel = std::move(other.return_channel);

			return *this;
		}

		// Callable tricks for non-member function calls
		template<class Fn>
		Task(Fn f, Args...args);

		template<class PtrToMember, class PtrToObject>
		Task(PtrToMember m, PtrToObject o, Args...args);

		void operator () () {
			unpackArguments(typename gens<sizeof...(Args)>::type());
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
