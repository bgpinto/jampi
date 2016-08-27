/*	TODO:
 *
 *	1. Melhorar os nomes dos parametros das templates;
 *
 *
 * */

#ifndef _POLICY_
#define _POLICY_

#include "Thread.h"

namespace parallel {

template< typename T , typename U >
class Policy {

	public:
		
		virtual void operator()(Task<U>& t) = 0; 
		virtual void sync() = 0;

		virtual ~Policy(){}

		Policy(const Policy&) = delete;
		Policy& operator = (const Policy&) = delete;
};
	

};

#endif
