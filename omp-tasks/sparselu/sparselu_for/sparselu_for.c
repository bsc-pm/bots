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
   for (i = 0; i < bots_arg_size_2; i++) 
   {
      for (j = 0; j < bots_arg_size_2; j++) 
      {
         if(M[i*bots_arg_size_2+j] != N[i*bots_arg_size_2+j])
         {
            if (bots_verbose_mode) fprintf(stderr, "Checking failure: A[%d][%d] = %f instead of B[%d][%d] = %f\n",i,j, M[i*bots_arg_size_2+j], i,j, N[i*bots_arg_size_2+j]);
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
   for (ii=0; ii < bots_arg_size_1; ii++)
   {
      for (jj=0; jj < bots_arg_size_1; jj++)
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
            M[ii*bots_arg_size_1+jj] = (float *) malloc(bots_arg_size_2*bots_arg_size_2*sizeof(float));
	    if ((M[ii*bots_arg_size_1+jj] == NULL))
            {
               fprintf(stderr, "Error: Out of memory\n");
               exit(101);
            }
            /* initializing matrix */
            p = M[ii*bots_arg_size_1+jj];
            for (i = 0; i < bots_arg_size_2; i++) 
            {
               for (j = 0; j < bots_arg_size_2; j++)
               {
	            init_val = (3125 * init_val) % 65536;
      	            (*p) = (float)((init_val - 32768.0) / 16384.0);
                    p++;
               }
            }
         }
         else
         {
            M[ii*bots_arg_size_1+jj] = NULL;
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
   for (ii = 0; ii < bots_arg_size_1; ii++) {
     for (jj = 0; jj < bots_arg_size_1; jj++) {
        if (M[ii*bots_arg_size_1+jj]!=NULL) printf ("x");
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

  p = (float *) malloc(bots_arg_size_2*bots_arg_size_2*sizeof(float));
  q=p;
  if (p!=NULL){
     for (i = 0; i < bots_arg_size_2; i++) 
        for (j = 0; j < bots_arg_size_2; j++){(*p)=0.0; p++;}
	
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

   for (k=0; k<bots_arg_size_2; k++)
      for (i=k+1; i<bots_arg_size_2; i++)
      {
         diag[i*bots_arg_size_2+k] = diag[i*bots_arg_size_2+k] / diag[k*bots_arg_size_2+k];
         for (j=k+1; j<bots_arg_size_2; j++)
            diag[i*bots_arg_size_2+j] = diag[i*bots_arg_size_2+j] - diag[i*bots_arg_size_2+k] * diag[k*bots_arg_size_2+j];
      }
}

/***********************************************************************
 * bdiv: 
 **********************************************************************/
void bdiv(float *diag, float *row)
{
   int i, j, k;
   for (i=0; i<bots_arg_size_2; i++)
      for (k=0; k<bots_arg_size_2; k++)
      {
         row[i*bots_arg_size_2+k] = row[i*bots_arg_size_2+k] / diag[k*bots_arg_size_2+k];
         for (j=k+1; j<bots_arg_size_2; j++)
            row[i*bots_arg_size_2+j] = row[i*bots_arg_size_2+j] - row[i*bots_arg_size_2+k]*diag[k*bots_arg_size_2+j];
      }
}
/***********************************************************************
 * bmod: 
 **********************************************************************/
void bmod(float *row, float *col, float *inner)
{
   int i, j, k;
   for (i=0; i<bots_arg_size_2; i++)
      for (j=0; j<bots_arg_size_2; j++)
         for (k=0; k<bots_arg_size_2; k++)
            inner[i*bots_arg_size_2+j] = inner[i*bots_arg_size_2+j] - row[i*bots_arg_size_2+k]*col[k*bots_arg_size_2+j];
}
/***********************************************************************
 * fwd: 
 **********************************************************************/
void fwd(float *diag, float *col)
{
   int i, j, k;
   for (j=0; j<bots_arg_size_2; j++)
      for (k=0; k<bots_arg_size_2; k++) 
         for (i=k+1; i<bots_arg_size_2; i++)
            col[i*bots_arg_size_2+j] = col[i*bots_arg_size_2+j] - diag[i*bots_arg_size_2+k]*col[k*bots_arg_size_2+j];
}
double sparselu_seq_call_old(float ***pSEQ)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **SEQ  = (float **) malloc(bots_arg_size_1*bots_arg_size_1*sizeof(float *));
   *pSEQ = SEQ;

   genmat(SEQ);
   if (bots_verbose_mode) print_structure("sequential", SEQ);

   start = bots_usecs();
   for (kk=0; kk<bots_arg_size_1; kk++) 
   {
      lu0(SEQ[kk*bots_arg_size_1+kk]);
      for (jj=kk+1; jj<bots_arg_size_1; jj++)
      {
         if (SEQ[kk*bots_arg_size_1+jj] != NULL)
         {
            fwd(SEQ[kk*bots_arg_size_1+kk], SEQ[kk*bots_arg_size_1+jj]);
         }
      }
      for (ii=kk+1; ii<bots_arg_size_1; ii++) 
      {
         if (SEQ[ii*bots_arg_size_1+kk] != NULL)
         {
            bdiv (SEQ[kk*bots_arg_size_1+kk], SEQ[ii*bots_arg_size_1+kk]);
         }
      }
      for (ii=kk+1; ii<bots_arg_size_1; ii++)
      {
         if (SEQ[ii*bots_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<bots_arg_size_1; jj++)
               if (SEQ[kk*bots_arg_size_1+jj] != NULL)
               {
                  if (SEQ[ii*bots_arg_size_1+jj]==NULL) SEQ[ii*bots_arg_size_1+jj] = allocate_clean_block();
                  bmod(SEQ[ii*bots_arg_size_1+kk], SEQ[kk*bots_arg_size_1+jj], SEQ[ii*bots_arg_size_1+jj]);
               }
      }
   }  
   end = bots_usecs();
   time = ((double)(end-start))/1000000;
   if (bots_verbose_mode) print_structure("sequential", SEQ);
   return time;
}
double sparselu_seq_call(float ***pBENCH)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **BENCH = (float **) malloc(bots_arg_size_1*bots_arg_size_1*sizeof(float *));
   *pBENCH = BENCH;

   genmat(BENCH);
   if (bots_verbose_mode) print_structure("sequential", BENCH);
   start = bots_usecs();
   for (kk=0; kk<bots_arg_size_1; kk++) 
   {
      lu0(BENCH[kk*bots_arg_size_1+kk]);
      for (jj=kk+1; jj<bots_arg_size_1; jj++)
         if (BENCH[kk*bots_arg_size_1+jj] != NULL)
         {
            fwd(BENCH[kk*bots_arg_size_1+kk], BENCH[kk*bots_arg_size_1+jj]);
         }
      for (ii=kk+1; ii<bots_arg_size_1; ii++) 
         if (BENCH[ii*bots_arg_size_1+kk] != NULL)
         {
            bdiv (BENCH[kk*bots_arg_size_1+kk], BENCH[ii*bots_arg_size_1+kk]);
         }
      for (ii=kk+1; ii<bots_arg_size_1; ii++)
         if (BENCH[ii*bots_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<bots_arg_size_1; jj++)
               if (BENCH[kk*bots_arg_size_1+jj] != NULL)
               {
                     if (BENCH[ii*bots_arg_size_1+jj]==NULL) BENCH[ii*bots_arg_size_1+jj] = allocate_clean_block();
                     bmod(BENCH[ii*bots_arg_size_1+kk], BENCH[kk*bots_arg_size_1+jj], BENCH[ii*bots_arg_size_1+jj]);
               }

   }

   end = bots_usecs();
   time = ((double)(end-start))/1000000;
   if (bots_verbose_mode) print_structure("sequential", BENCH);
   return time;
}
double sparselu_par_call(float ***pBENCH)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **BENCH = (float **) malloc(bots_arg_size_1*bots_arg_size_1*sizeof(float *));
   *pBENCH = BENCH;

   genmat(BENCH);
   if (bots_verbose_mode) print_structure("benchmark", BENCH);
   start = bots_usecs();
#pragma omp parallel private(kk)
   {
   for (kk=0; kk<bots_arg_size_1; kk++) 
   {
#pragma omp single
      lu0(BENCH[kk*bots_arg_size_1+kk]);

#pragma omp for nowait
      for (jj=kk+1; jj<bots_arg_size_1; jj++)
         if (BENCH[kk*bots_arg_size_1+jj] != NULL)
            #pragma omp task untied firstprivate(kk, jj) shared(BENCH)
         {
            fwd(BENCH[kk*bots_arg_size_1+kk], BENCH[kk*bots_arg_size_1+jj]);
         }
#pragma omp for
      for (ii=kk+1; ii<bots_arg_size_1; ii++) 
         if (BENCH[ii*bots_arg_size_1+kk] != NULL)
            #pragma omp task untied firstprivate(kk, ii) shared(BENCH)
         {
            bdiv (BENCH[kk*bots_arg_size_1+kk], BENCH[ii*bots_arg_size_1+kk]);
         }

#pragma omp for private(jj)
      for (ii=kk+1; ii<bots_arg_size_1; ii++)
         if (BENCH[ii*bots_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<bots_arg_size_1; jj++)
               if (BENCH[kk*bots_arg_size_1+jj] != NULL)
               #pragma omp task untied firstprivate(kk, jj, ii) shared(BENCH)
               {
                     if (BENCH[ii*bots_arg_size_1+jj]==NULL) BENCH[ii*bots_arg_size_1+jj] = allocate_clean_block();
                     bmod(BENCH[ii*bots_arg_size_1+kk], BENCH[kk*bots_arg_size_1+jj], BENCH[ii*bots_arg_size_1+jj]);
               }

   }
   }

   end = bots_usecs();
   time = ((double)(end-start))/1000000;
   if (bots_verbose_mode) print_structure("benchmark", BENCH);
   return time;
}
int sparselu_check(float **SEQ, float **BENCH)
{
   int ii,jj,ok=1;

   for (ii=0; ((ii<bots_arg_size_1) && ok); ii++)
   {
      for (jj=0; ((jj<bots_arg_size_1) && ok); jj++)
      {
         if ((SEQ[ii*bots_arg_size_1+jj] == NULL) && (BENCH[ii*bots_arg_size_1+jj] != NULL)) ok = FALSE;
         if ((SEQ[ii*bots_arg_size_1+jj] != NULL) && (BENCH[ii*bots_arg_size_1+jj] == NULL)) ok = FALSE;
         if ((SEQ[ii*bots_arg_size_1+jj] != NULL) && (BENCH[ii*bots_arg_size_1+jj] != NULL))
            ok = checkmat(SEQ[ii*bots_arg_size_1+jj], BENCH[ii*bots_arg_size_1+jj]);
      }
   }
   if (ok) return BOTS_RESULT_SUCCESSFUL;
   else return BOTS_RESULT_UNSUCCESSFUL;
}

