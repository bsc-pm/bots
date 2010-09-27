/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

/*
 * Original code from the Cilk project
 *
 * Copyright (c) 2000 Massachusetts Institute of Technology
 * Copyright (c) 2000 Matteo Frigo
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "app-desc.h"
#include "bots.h"

int best_so_far;
int number_of_tasks;

#pragma omp threadprivate(number_of_tasks)

int compare(struct item *a, struct item *b)
{
     double c = ((double) a->value / a->weight) -
     ((double) b->value / b->weight);

     if (c > 0) return -1;
     if (c < 0) return 1;
     return 0;
}

int read_input(const char *filename, struct item *items, int *capacity, int *n)
{
     int i;
     FILE *f;

     if (filename == NULL) filename = "\0";
     f = fopen(filename, "r");
     if (f == NULL) {
	  fprintf(stderr, "open_input(\"%s\") failed\n", filename);
	  return -1;
     }
     /* format of the input: #items capacity value1 weight1 ... */
     fscanf(f, "%d", n);
     fscanf(f, "%d", capacity);

     for (i = 0; i < *n; ++i)
	  fscanf(f, "%d %d", &items[i].value, &items[i].weight);

     fclose(f);

     /* sort the items on decreasing order of value/weight */
     /* cilk2c is fascist in dealing with pointers, whence the ugly cast */
     qsort(items, *n, sizeof(struct item), (int (*)(const void *, const void *)) compare);

     return 0;
}

/* 
 * return the optimal solution for n items (first is e) and
 * capacity c. Value so far is v.
 */
#if defined(IF_CUTOFF)
void knapsack_par(struct item *e, int c, int n, int v, int *sol, int l)
{
     int with, without, best;
     double ub;

     number_of_tasks++;
     /* base case: full knapsack or no items */
     if (c < 0)
     {
         *sol = INT_MIN;
         return;
     }

     /* feasible solution, with value v */
     if (n == 0 || c == 0)
     {
         *sol = v;
         return;
     }

     ub = (double) v + c * e->value / e->weight;

     if (ub < best_so_far) {
	  /* prune ! */
          *sol = INT_MIN;
          return;
     }
     /* 
      * compute the best solution without the current item in the knapsack 
      */
     #pragma omp task untied firstprivate(e,c,n,v,l) shared(without) if (l < bots_cutoff_value)
     knapsack_par(e + 1, c, n - 1, v, &without,l+1);

     /* compute the best solution with the current item in the knapsack */
     #pragma omp task untied firstprivate(e,c,n,v,l) shared(with)  if (l < bots_cutoff_value)
     knapsack_par(e + 1, c - e->weight, n - 1, v + e->value, &with,l+1);

     #pragma omp taskwait
     best = with > without ? with : without;

     /* 
      * notice the race condition here. The program is still
      * correct, in the sense that the best solution so far
      * is at least best_so_far. Moreover best_so_far gets updated
      * when returning, so eventually it should get the right
      * value. The program is highly non-deterministic.
      */
     if (best > best_so_far) best_so_far = best;

     *sol = best;
}
#elif defined (MANUAL_CUTOFF)
void knapsack_par(struct item *e, int c, int n, int v, int *sol, int l)
{
     int with, without, best;
     double ub;

     number_of_tasks++;
     /* base case: full knapsack or no items */
     if (c < 0)
     {
         *sol = INT_MIN;
         return;
     }

     /* feasible solution, with value v */
     if (n == 0 || c == 0)
     {
         *sol = v;
         return;
     }

     ub = (double) v + c * e->value / e->weight;

     if (ub < best_so_far) {
	  /* prune ! */
          *sol = INT_MIN;
          return;
     }
     if (l < bots_cutoff_value)
     {
        /* compute the best solution without the current item in the knapsack */
        #pragma omp task untied firstprivate(e,c,n,v,l) shared(without)
        knapsack_par(e + 1, c, n - 1, v, &without,l+1);

        /* compute the best solution with the current item in the knapsack */
        #pragma omp task untied firstprivate(e,c,n,v,l) shared(with)
        knapsack_par(e + 1, c - e->weight, n - 1, v + e->value, &with,l+1);

        #pragma omp taskwait
     }
     else
     {
        /* compute the best solution without the current item in the knapsack */
        knapsack_seq(e + 1, c, n - 1, v, &without);
        /* compute the best solution with the current item in the knapsack */
        knapsack_seq(e + 1, c - e->weight, n - 1, v + e->value, &with);
     }

     best = with > without ? with : without;

     /* 
      * notice the race condition here. The program is still
      * correct, in the sense that the best solution so far
      * is at least best_so_far. Moreover best_so_far gets updated
      * when returning, so eventually it should get the right
      * value. The program is highly non-deterministic.
      */
     if (best > best_so_far) best_so_far = best;

     *sol = best;
}
#else
void knapsack_par(struct item *e, int c, int n, int v, int *sol, int l)
{
     int with, without, best;
     double ub;

     number_of_tasks++;
     /* base case: full knapsack or no items */
     if (c < 0)
     {
         *sol = INT_MIN;
         return;
     }

     /* feasible solution, with value v */
     if (n == 0 || c == 0)
     {
         *sol = v;
         return;
     }

     ub = (double) v + c * e->value / e->weight;

     if (ub < best_so_far) {
	  /* prune ! */
          *sol = INT_MIN;
          return;
     }
     /* 
      * compute the best solution without the current item in the knapsack 
      */
     #pragma omp task untied firstprivate(e,c,n,v,l) shared(without)
     knapsack_par(e + 1, c, n - 1, v, &without,l+1);

     /* compute the best solution with the current item in the knapsack */
     #pragma omp task untied firstprivate(e,c,n,v,l) shared(with)
     knapsack_par(e + 1, c - e->weight, n - 1, v + e->value, &with,l+1);

     #pragma omp taskwait
     best = with > without ? with : without;

     /* 
      * notice the race condition here. The program is still
      * correct, in the sense that the best solution so far
      * is at least best_so_far. Moreover best_so_far gets updated
      * when returning, so eventually it should get the right
      * value. The program is highly non-deterministic.
      */
     if (best > best_so_far) best_so_far = best;

     *sol = best;
}
#endif
void knapsack_seq(struct item *e, int c, int n, int v, int *sol)
{
     int with, without, best;
     double ub;

     number_of_tasks++;
     /* base case: full knapsack or no items */
     if (c < 0)
     {
         *sol = INT_MIN;
         return;
     }

     /* feasible solution, with value v */
     if (n == 0 || c == 0)
     {
         *sol = v;
         return;
     }

     ub = (double) v + c * e->value / e->weight;

     if (ub < best_so_far) {
	  /* prune ! */
          *sol = INT_MIN;
          return;
     }
     /* 
      * compute the best solution without the current item in the knapsack 
      */
     knapsack_seq(e + 1, c, n - 1, v, &without);

     /* compute the best solution with the current item in the knapsack */
     knapsack_seq(e + 1, c - e->weight, n - 1, v + e->value, &with);

     best = with > without ? with : without;

     /* 
      * notice the race condition here. The program is still
      * correct, in the sense that the best solution so far
      * is at least best_so_far. Moreover best_so_far gets updated
      * when returning, so eventually it should get the right
      * value. The program is highly non-deterministic.
      */
     if (best > best_so_far) best_so_far = best;

     *sol = best;
}
void knapsack_main_par (struct item *e, int c, int n, int *sol)
{
     best_so_far = INT_MIN;

     #pragma omp parallel
     {
        number_of_tasks = 0;
        #pragma omp single
        #pragma omp task untied
        {
           knapsack_par(e, c, n, 0, sol, 0);
        }

        #pragma omp critical
        bots_number_of_tasks += number_of_tasks;
     }
     if (bots_verbose_mode) printf("Best value for parallel execution is %d\n\n", *sol);
}
void knapsack_main_seq (struct item *e, int c, int n, int *sol)
{
     best_so_far = INT_MIN;
     number_of_tasks = 0;

     knapsack_seq(e, c, n, 0, sol);

     if (bots_verbose_mode) printf("Best value for sequential execution is %d\n\n", *sol);
}

int  knapsack_check (int sol_seq, int sol_par)
{
   if (sol_seq == sol_par) return BOTS_RESULT_SUCCESSFUL;
   else return BOTS_RESULT_UNSUCCESSFUL;
}
