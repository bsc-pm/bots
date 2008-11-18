#include "nbs.h"

int fib_seq (int n)
{
	int x, y;
	if (n < 2) return n;

	x = fib_seq(n - 1);
	y = fib_seq(n - 2);

	return x + y;
}

#if defined(IF_CUTOFF)

int fib (int n)
{
	int x, y;
	if (n < 2) return n;

	#pragma omp task untied shared(x) firstprivate(n) if(n < nbs_cutoff_value)
	x = fib(n - 1);

	#pragma omp task untied shared(y) firstprivate(n) if(n < nbs_cutoff_value)
	y = fib(n - 2);

	#pragma omp taskwait
	return x + y;
}

#elif defined(MANUAL_CUTOFF)

int fib (int n)
{
	int x, y;
	if (n < 2) return n;

	if ( n < nbs_cutoff_value ) {
		#pragma omp task untied shared(x) firstprivate(n)
		x = fib(n - 1);

		#pragma omp task untied shared(y) firstprivate(n)
		y = fib(n - 2);

		#pragma omp taskwait
	} else {
		x = fib_seq(n-1);
		y = fib_seq(n-2);
	}

	return x + y;
}

#else

int fib (int n)
{
	int x, y;
	if (n < 2) return n;

	#pragma omp task untied shared(x) firstprivate(n)
	x = fib(n - 1);
	#pragma omp task untied shared(y) firstprivate(n)
	y = fib(n - 2);

	#pragma omp taskwait
	return x + y;
}

#endif

static int par_res, seq_res;

void fib0 (int n)
{
	par_res = fib(n);
	printf("Fibonacci result for %d is %d\n",n,par_res);
}

void fib0_seq (int n)
{
	seq_res = fib_seq(n);
	printf("Fibonacci result for %d is %d\n",n,seq_res);
}

int fib_verify ()
{
	return par_res == seq_res;
}

