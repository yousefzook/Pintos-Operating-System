#include "real.h"
#include <stdint.h>

// convert from int to real and viceversa
int real_to_int(real i){
	return i.value/power(2,14);
}
real int_to_real(int i){
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
	result.value = ((int64_t)x.value)*y.value/power(2,14);
	return result;
}
real div(real x, real y){
	real result;
	result.value = ((int64_t)x.value)*power(2,14)/y.value;
	return result;
}

int power(int base, int exponent){
	int i;
	int result = 1;
	for (i = 0; i < exponent; i++){
		result = base*result;
	}
	return result;
}
