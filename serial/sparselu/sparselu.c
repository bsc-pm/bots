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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <libgen.h>
#include "bots.h"

/***********************************************************************
 * checkmat: 
 **********************************************************************/
int checkmat (float *M, float *N)
{
   int i, j;
   for (i = 0; i < bots_arg_size_1; i++) 
   {
      for (j = 0; j < bots_arg_size_1; j++) 
      {
         if(M[i*bots_arg_size_1+j] != N[i*bots_arg_size_1+j])
         {
            if (bots_verbose_mode >= BOTS_VERBOSE_DEBUG) fprintf(stderr, "Checking failure: A[%d][%d] = %f instead of B[%d][%d] = %f\n",i,j, M[i*bots_arg_size_1+j], i,j, N[i*bots_arg_size_1+j]);
            return FALSE;
         }
      }
   }
   return TRUE;
}
/***********************************************************************
 * genmat: 
 **********************************************************************/
void genmat (float *M[])
{
   int null_entry, init_val, i, j, ii, jj;
   float *p;

   init_val = 1325;

   /* generating the structure */
   for (ii=0; ii < bots_arg_size; ii++)
   {
      for (jj=0; jj < bots_arg_size; jj++)
      {
         /* computing null entries */
         null_entry=FALSE;
         if ((ii<jj) && (ii%3 !=0)) null_entry = TRUE;
         if ((ii>jj) && (jj%3 !=0)) null_entry = TRUE;
	 if (ii%2==1) null_entry = TRUE;
	 if (jj%2==1) null_entry = TRUE;
	 if (ii==jj) null_entry = FALSE;
	 if (ii==jj-1) null_entry = FALSE;
         if (ii-1 == jj) null_entry = FALSE; 
         /* allocating matrix */
         if (null_entry == FALSE){
            M[ii*bots_arg_size+jj] = (float *) malloc(bots_arg_size_1*bots_arg_size_1*sizeof(float));
	    if ((M[ii*bots_arg_size+jj] == NULL))
            {
               fprintf(stderr, "Error: Out of memory\n");
               exit(101);
            }
            /* initializing matrix */
            p = M[ii*bots_arg_size+jj];
            for (i = 0; i < bots_arg_size_1; i++) 
            {
               for (j = 0; j < bots_arg_size_1; j++)
               {
	            init_val = (3125 * init_val) % 65536;
      	            (*p) = (float)((init_val - 32768.0) / 16384.0);
                    p++;
               }
            }
         }
         else
         {
            M[ii*bots_arg_size+jj] = NULL;
         }
      }
   }
}
/***********************************************************************
 * print_structure: 
 **********************************************************************/
void print_structure(char *name, float *M[])
{
   int ii, jj;
   printf ("Structure for matrix %s @ 0x%p\n",name, M);
   for (ii = 0; ii < bots_arg_size; ii++) {
     for (jj = 0; jj < bots_arg_size; jj++) {
        if (M[ii*bots_arg_size+jj]!=NULL) printf ("x");
        else printf (" ");
     }
     printf ("\n");
   }
   printf ("\n");
}
/***********************************************************************
 * allocate_clean_block: 
 **********************************************************************/
float * allocate_clean_block()
{
  int i,j;
  float *p, *q;

  p = (float *) malloc(bots_arg_size_1*bots_arg_size_1*sizeof(float));
  q=p;
  if (p!=NULL){
     for (i = 0; i < bots_arg_size_1; i++) 
        for (j = 0; j < bots_arg_size_1; j++){(*p)=0.0; p++;}
	
  }
  else
  {
      printf ("Error: Out of memory\n");
      exit (101);
  }
  return (q);
}

/***********************************************************************
 * lu0: 
 **********************************************************************/
void lu0(float *diag)
{
   int i, j, k;

   for (k=0; k<bots_arg_size_1; k++)
      for (i=k+1; i<bots_arg_size_1; i++)
      {
         diag[i*bots_arg_size_1+k] = diag[i*bots_arg_size_1+k] / diag[k*bots_arg_size_1+k];
         for (j=k+1; j<bots_arg_size_1; j++)
            diag[i*bots_arg_size_1+j] = diag[i*bots_arg_size_1+j] - diag[i*bots_arg_size_1+k] * diag[k*bots_arg_size_1+j];
      }
}

/***********************************************************************
 * bdiv: 
 **********************************************************************/
void bdiv(float *diag, float *row)
{
   int i, j, k;
   for (i=0; i<bots_arg_size_1; i++)
      for (k=0; k<bots_arg_size_1; k++)
      {
         row[i*bots_arg_size_1+k] = row[i*bots_arg_size_1+k] / diag[k*bots_arg_size_1+k];
         for (j=k+1; j<bots_arg_size_1; j++)
            row[i*bots_arg_size_1+j] = row[i*bots_arg_size_1+j] - row[i*bots_arg_size_1+k]*diag[k*bots_arg_size_1+j];
      }
}
/***********************************************************************
 * bmod: 
 **********************************************************************/
void bmod(float *row, float *col, float *inner)
{
   int i, j, k;
   for (i=0; i<bots_arg_size_1; i++)
      for (j=0; j<bots_arg_size_1; j++)
         for (k=0; k<bots_arg_size_1; k++)
            inner[i*bots_arg_size_1+j] = inner[i*bots_arg_size_1+j] - row[i*bots_arg_size_1+k]*col[k*bots_arg_size_1+j];
}
/***********************************************************************
 * fwd: 
 **********************************************************************/
void fwd(float *diag, float *col)
{
   int i, j, k;
   for (j=0; j<bots_arg_size_1; j++)
      for (k=0; k<bots_arg_size_1; k++) 
         for (i=k+1; i<bots_arg_size_1; i++)
            col[i*bots_arg_size_1+j] = col[i*bots_arg_size_1+j] - diag[i*bots_arg_size_1+k]*col[k*bots_arg_size_1+j];
}
double sparselu(float ***pBENCH)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **BENCH = (float **) malloc(bots_arg_size*bots_arg_size*sizeof(float *));
   *pBENCH = BENCH;

   genmat(BENCH);
   if (bots_verbose_mode >= BOTS_VERBOSE_DEFAULT) print_structure("sequential", BENCH);
   start = bots_usecs();
   for (kk=0; kk<bots_arg_size; kk++) 
   {
      lu0(BENCH[kk*bots_arg_size+kk]);
      for (jj=kk+1; jj<bots_arg_size; jj++)
         if (BENCH[kk*bots_arg_size+jj] != NULL)
         {
            fwd(BENCH[kk*bots_arg_size+kk], BENCH[kk*bots_arg_size+jj]);
         }
      for (ii=kk+1; ii<bots_arg_size; ii++) 
         if (BENCH[ii*bots_arg_size+kk] != NULL)
         {
            bdiv (BENCH[kk*bots_arg_size+kk], BENCH[ii*bots_arg_size+kk]);
         }
      for (ii=kk+1; ii<bots_arg_size; ii++)
         if (BENCH[ii*bots_arg_size+kk] != NULL)
            for (jj=kk+1; jj<bots_arg_size; jj++)
               if (BENCH[kk*bots_arg_size+jj] != NULL)
               {
                     if (BENCH[ii*bots_arg_size+jj]==NULL) BENCH[ii*bots_arg_size+jj] = allocate_clean_block();
                     bmod(BENCH[ii*bots_arg_size+kk], BENCH[kk*bots_arg_size+jj], BENCH[ii*bots_arg_size+jj]);
               }

   }

   end = bots_usecs();
   time = ((double)(end-start))/1000000;
   if (bots_verbose_mode >= BOTS_VERBOSE_DEFAULT) print_structure("sequential", BENCH);
   return time;
}

