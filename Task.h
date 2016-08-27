#ifndef _TASK_
#define _TASK_

#include <memory>
#include "Callable.h"

namespace parallel {

template
<
	typename ReturnType,
	typename ...Args
>
class Task {
	
	// Talvez seja o caso de task gerenciar os parametros tambem
	std::unique_ptr< Callable<ReturnType, Args...> > function;

	public:
		
		Task() = default;
		~Task(){}
		
		// O objeto copiado eh destruido	
		Task(Task& t) {
		       function.reset(t.function.release());
		 		
		}

		Task& operator = (Task& t) {
			function.reset(t.function.release()); 
			return *this;
		}
		
		// Callable tricks for non-member function calls
		template<class Fn>
		Task(Fn f);

		template<class PtrToMember, class PtrToObject>
		Task(PtrToMember m, PtrToObject o);

		// here it should be a member function call

		// the interface - question now is: wheter or not and how to store args;
		ReturnType operator () (Args...args) {
			if (function) return (*function)(args...); // virtual fowarding call to callable stuff while preserving conversions
		}

};

template
<
	typename ReturnType,
	typename ...Args

>
template<class Fn>
Task<ReturnType, Args...>::Task(Fn f):function( new CallableImpl<Fn, ReturnType, Args...>(f) )
{}


template
<
	typename ReturnType,
	typename ...Args
>
template<class PtrToMember, class PtrToObject>
Task<ReturnType, Args...>::Task(PtrToMember m, PtrToObject o):function( new MemberCallableImpl<PtrToMember, PtrToObject, ReturnType, Args...>(m, o) )
{}


};

#endif
