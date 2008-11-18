/*
 * 
 *
 * Original Author: Keith Randall
 */

/*
 * Copyright (c) 2000 Massachusetts Institute of Technology
 * Copyright (c) 2000 Matteo Frigo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "nbs.h"


/* Checking information */

static int solutions[] = {
        1,
        0,
        0,
        2,
        10, /* 5 */
        4,
        40,
        92,
        352,
        724, /* 10 */
        2680,
        14200,
        73712,
        365596,
};
#define MAX_SOLUTIONS sizeof(solutions)/sizeof(int)

int mycount;
#pragma omp threadprivate(mycount)

int total_count;


/*
 * <a> contains array of <n> queen positions.  Returns 1
 * if none of the queens conflict, and returns 0 otherwise.
 */
int ok(int n, char *a)
{
     int i, j;
     char p, q;

     for (i = 0; i < n; i++) {
	  p = a[i];

	  for (j = i + 1; j < n; j++) {
	       q = a[j];
	       if (q == p || q == p - (j - i) || q == p + (j - i))
		    return 0;
	  }
     }
     return 1;
}

void nqueens_ser (int n, int j, char *a, int *solutions)
{
	int i,res;

	if (n == j) {
		/* good solution, count it */
#ifndef FORCE_TIED_TASKS
		*solutions = 1;
#else
		mycount++;
#endif
		return;
	}

	*solutions = 0;


     	/* try each possible position for queen <j> */
	for (i = 0; i < n; i++) {
		{
	  		/* allocate a temporary array and copy <a> into it */
	  		char * b = alloca((j + 1) * sizeof(char));
	  		memcpy(b, a, j * sizeof(char));
	  		b[j] = i;
	  		if (ok(j + 1, b)) {
	       			nqueens_ser(n, j + 1, b,&res);
#ifndef FORCE_TIED_TASKS
				*solutions += res;
#endif
			}
		}
	}
}

#if defined(IF_CUTOFF)

void nqueens(int n, int j, char *a, int *solutions, int depth)
{
	int i;
	int *csols;


	if (n == j) {
		/* good solution, count it */
#ifndef FORCE_TIED_TASKS
		*solutions = 1;
#else
		mycount++;
#endif
		return;
	}

	*solutions = 0;

#ifndef FORCE_TIED_TASKS
	csols = alloca(n*sizeof(int));
	memset(csols,0,n*sizeof(int));
#endif

     	/* try each possible position for queen <j> */
	for (i = 0; i < n; i++) {
 		#pragma omp task untied if(depth < nbs_cutoff_value)
		{
	  		/* allocate a temporary array and copy <a> into it */
	  		char * b = alloca((j + 1) * sizeof(char));
	  		memcpy(b, a, j * sizeof(char));
	  		b[j] = i;
	  		if (ok(j + 1, b))
	       			nqueens(n, j + 1, b,&csols[i],depth+1);
		}
	}
	#pragma omp taskwait

#ifndef FORCE_TIED_TASKS
	for ( i = 0; i < n; i++) *solutions += csols[i];
#endif
}


#elif defined(MANUAL_CUTOFF)

void nqueens(int n, int j, char *a, int *solutions, int depth)
{
	int i;
	int *csols;


	if (n == j) {
		/* good solution, count it */
#ifndef FORCE_TIED_TASKS
		*solutions = 1;
#else
		mycount++;
#endif
		return;
	}

	*solutions = 0;

#ifndef FORCE_TIED_TASKS
	csols = alloca(n*sizeof(int));
	memset(csols,0,n*sizeof(int));
#endif

     	/* try each possible position for queen <j> */
	for (i = 0; i < n; i++) {
		if ( depth < nbs_cutoff_value ) {
 			#pragma omp task untied
			{
	  			/* allocate a temporary array and copy <a> into it */
	  			char * b = alloca((j + 1) * sizeof(char));
	  			memcpy(b, a, j * sizeof(char));
	  			b[j] = i;
	  			if (ok(j + 1, b))
	       				nqueens(n, j + 1, b,&csols[i],depth+1);
			}
		} else {
  			char * b = alloca((j + 1) * sizeof(char));
 			memcpy(b, a, j * sizeof(char));
  			b[j] = i;
  			if (ok(j + 1, b))
       				nqueens_ser(n, j + 1, b,&csols[i]);
		}
	}
	#pragma omp taskwait

#ifndef FORCE_TIED_TASKS
	for ( i = 0; i < n; i++) *solutions += csols[i];
#endif
}


#else 

void nqueens(int n, int j, char *a, int *solutions, int depth)
{
	int i;
	int *csols;


	if (n == j) {
		/* good solution, count it */
#ifndef FORCE_TIED_TASKS
		*solutions = 1;
#else
		mycount++;
#endif
		return;
	}

	*solutions = 0;

#ifndef FORCE_TIED_TASKS
	csols = alloca(n*sizeof(int));
	memset(csols,0,n*sizeof(int));
#endif

     	/* try each possible position for queen <j> */
	for (i = 0; i < n; i++) {
 		#pragma omp task untied
		{
	  		/* allocate a temporary array and copy <a> into it */
	  		char * b = alloca((j + 1) * sizeof(char));
	  		memcpy(b, a, j * sizeof(char));
	  		b[j] = i;
	  		if (ok(j + 1, b))
	       			nqueens(n, j + 1, b,&csols[i],depth);
		}
	}
	#pragma omp taskwait

#ifndef FORCE_TIED_TASKS
	for ( i = 0; i < n; i++) *solutions += csols[i];
#endif
}

#endif

void find_queens_ser (int size)
{
	char *a;

	total_count=0;
	a = alloca(size * sizeof(char));
	nqueens_ser(size, 0, a, &total_count);
}

void find_queens (int size)
{
	total_count=0;

	#pragma omp parallel
	{
		#pragma omp single
		{
			char *a;

			a = alloca(size * sizeof(char));
			nqueens(size, 0, a, &total_count,0);
		}
#ifdef FORCE_TIED_TASKS
		#pragma omp atomic
			total_count += mycount;
#endif
	}
}


int verify_queens (int size)
{
	if ( size > MAX_SOLUTIONS ) return NBS_RESULT_NA;


	if ( total_count == solutions[size-1]) return NBS_RESULT_SUCCESSFUL;

	return NBS_RESULT_UNSUCCESSFUL;
}
