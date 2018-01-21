/* define new data type called real -float-like data type- */
typedef struct 
{
	int value;
}real;

/* convert from int to real and viceversa. */
int real_to_int(real i);
real int_to_real(int i);

/* math operations */
real add(real x, real y);
real sub(real x, real y);
real mul(real x, real y);
real div(real x, real y);

/* power function return base^exponent */
int power(int base, int exponent);
