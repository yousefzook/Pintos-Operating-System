#include "real.h"

// convert from int to real and viceversa
unsigned real_to_int(real i){
	return i.value/power(2,14);
}
real int_to_real(unsigned i){
	real result;
	result.value = i*power(2,14);
	return result;
}

// implementing math operations on real types
real add(real x, real y){
	real result;
	result.value = x.value + y.value;
	return result;
}
real sub(real x, real y){
	real result;
	result.value = x.value - y.value;
	return result;
}
real mul(real x, real y){
	real result;
	result.value = x.value*(y.value/power(2,14));
	return result;
}
real div(real x, real y){
	real result;
	result.value = x.value/(y.value/power(2,14));
	return result;
}

unsigned power(unsigned base, unsigned exponent){
	unsigned i;
	unsigned result = 1;
	for (i = 0; i < exponent; i++){
		result = base*result;
	}
	return result;
}
