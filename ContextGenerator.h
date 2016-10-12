#ifndef PARALLEL_CONTEXTGENERATOR
#define PARALLEL_CONTEXTGENERATOR

#include <functional>
#include <string>

namespace parallel { 

std::size_t generate_context(int context) { 
	std::hash<std::string> ctx_generator;
	return ctx_generator(std::to_string(context * 33 * 29 )); 
}


};


#endif
