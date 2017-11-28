// define new data type called real -float-like data type-
typedef struct 
{
	unsigned value;
}real;

// convert from int to real and viceversa
unsigned real_to_int(real i);
real int_to_real(unsigned i);

// math operations
real add(real x, real y);
real sub(real x, real y);
real mul(real x, real y);
real div(real x, real y);

// power function return base^exponent
unsigned power(unsigned base, unsigned exponent);
