#ifndef _REDUCE_
#define _REDUCE_

#include <type_traits>
#include <vector>
#include <atomic>
#include <iostream>


#include "default_thread.h"
#include "SchedulingPolicy.h"


namespace parallel {


template
<
	typename SchedulerType,
	template <typename> class ThreadType,
	typename ReductionOperator,
	typename T	
>
T reduce_helper(SchedulerType& S, std::vector<T>& collection, int size) {
	
	if (size == 1) {
		return collection[0];
	} else {
		
		int level_index = size;

		if ((level_index % 2)!= 0) {
			ReductionOperator op;
			level_index = level_index - 1;
			collection[0] = op( collection[0], collection[level_index] );
		}

		level_index = level_index / 2;

		for (int i = 0; i < level_index; i++ ) {

			parallel::Task<void> reduce_job([i, level_index, &collection]( )->void{
				
				ReductionOperator op;	

				int index = i + level_index;
				
				collection[i] = op(collection[i], collection[index]);

			});

			using TASK = decltype(reduce_job);

			S.template spawn< TASK, ThreadType<TASK> >(reduce_job, level_index);
		}

		 S.template sync(level_index);

 		 return reduce_helper<SchedulerType, ThreadType, ReductionOperator, T >(S, collection, level_index);	

	}

	return T();
}


template
<
	typename SchedAlg,
	typename ReductionOperator, 
	typename T,
	template <typename> class ThreadType = DefaultThread
>	
T reduce(std::vector<T>& collection) { 

	parallel::SchedulingPolicy<SchedAlg> scheduler;	

	return reduce_helper<decltype(scheduler), ThreadType, ReductionOperator, T>(scheduler, collection, collection.size()); 
}



};
#endif
