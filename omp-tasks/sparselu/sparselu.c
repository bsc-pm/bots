#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <libgen.h>
#include "nbs.h"

/***********************************************************************
 * checkmat: 
 **********************************************************************/
int checkmat (float *M, float *N)
{
   int i, j;
   for (i = 0; i < nbs_arg_size_2; i++) 
   {
      for (j = 0; j < nbs_arg_size_2; j++) 
      {
         if(M[i*nbs_arg_size_2+j] != N[i*nbs_arg_size_2+j])
         {
            if (nbs_verbose_mode) fprintf(stderr, "Checking failure: A[%d][%d] = %f instead of B[%d][%d] = %f\n",i,j, M[i*nbs_arg_size_2+j], i,j, N[i*nbs_arg_size_2+j]);
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
   for (ii=0; ii < nbs_arg_size_1; ii++)
   {
      for (jj=0; jj < nbs_arg_size_1; jj++)
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
            M[ii*nbs_arg_size_1+jj] = (float *) malloc(nbs_arg_size_2*nbs_arg_size_2*sizeof(float));
	    if ((M[ii*nbs_arg_size_1+jj] == NULL))
            {
               fprintf(stderr, "Error: Out of memory\n");
               exit(101);
            }
            /* initializing matrix */
            p = M[ii*nbs_arg_size_1+jj];
            for (i = 0; i < nbs_arg_size_2; i++) 
            {
               for (j = 0; j < nbs_arg_size_2; j++)
               {
	            init_val = (3125 * init_val) % 65536;
      	            (*p) = (float)((init_val - 32768.0) / 16384.0);
                    p++;
               }
            }
         }
         else
         {
            M[ii*nbs_arg_size_1+jj] = NULL;
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
   for (ii = 0; ii < nbs_arg_size_1; ii++) {
     for (jj = 0; jj < nbs_arg_size_1; jj++) {
        if (M[ii*nbs_arg_size_1+jj]!=NULL) printf ("x");
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

  p = (float *) malloc(nbs_arg_size_2*nbs_arg_size_2*sizeof(float));
  q=p;
  if (p!=NULL){
     for (i = 0; i < nbs_arg_size_2; i++) 
        for (j = 0; j < nbs_arg_size_2; j++){(*p)=0.0; p++;}
	
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

   for (k=0; k<nbs_arg_size_2; k++)
      for (i=k+1; i<nbs_arg_size_2; i++)
      {
         diag[i*nbs_arg_size_2+k] = diag[i*nbs_arg_size_2+k] / diag[k*nbs_arg_size_2+k];
         for (j=k+1; j<nbs_arg_size_2; j++)
            diag[i*nbs_arg_size_2+j] = diag[i*nbs_arg_size_2+j] - diag[i*nbs_arg_size_2+k] * diag[k*nbs_arg_size_2+j];
      }
}

/***********************************************************************
 * bdiv: 
 **********************************************************************/
void bdiv(float *diag, float *row)
{
   int i, j, k;
   for (i=0; i<nbs_arg_size_2; i++)
      for (k=0; k<nbs_arg_size_2; k++)
      {
         row[i*nbs_arg_size_2+k] = row[i*nbs_arg_size_2+k] / diag[k*nbs_arg_size_2+k];
         for (j=k+1; j<nbs_arg_size_2; j++)
            row[i*nbs_arg_size_2+j] = row[i*nbs_arg_size_2+j] - row[i*nbs_arg_size_2+k]*diag[k*nbs_arg_size_2+j];
      }
}
/***********************************************************************
 * bmod: 
 **********************************************************************/
void bmod(float *row, float *col, float *inner)
{
   int i, j, k;
   for (i=0; i<nbs_arg_size_2; i++)
      for (j=0; j<nbs_arg_size_2; j++)
         for (k=0; k<nbs_arg_size_2; k++)
            inner[i*nbs_arg_size_2+j] = inner[i*nbs_arg_size_2+j] - row[i*nbs_arg_size_2+k]*col[k*nbs_arg_size_2+j];
}
/***********************************************************************
 * fwd: 
 **********************************************************************/
void fwd(float *diag, float *col)
{
   int i, j, k;
   for (j=0; j<nbs_arg_size_2; j++)
      for (k=0; k<nbs_arg_size_2; k++) 
         for (i=k+1; i<nbs_arg_size_2; i++)
            col[i*nbs_arg_size_2+j] = col[i*nbs_arg_size_2+j] - diag[i*nbs_arg_size_2+k]*col[k*nbs_arg_size_2+j];
}
double sparselu_seq_call_old(float ***pSEQ)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **SEQ  = (float **) malloc(nbs_arg_size_1*nbs_arg_size_1*sizeof(float *));
   *pSEQ = SEQ;

   genmat(SEQ);
   if (nbs_verbose_mode) print_structure("sequential", SEQ);

   start = nbs_usecs();
   for (kk=0; kk<nbs_arg_size_1; kk++) 
   {
      lu0(SEQ[kk*nbs_arg_size_1+kk]);
      for (jj=kk+1; jj<nbs_arg_size_1; jj++)
      {
         if (SEQ[kk*nbs_arg_size_1+jj] != NULL)
         {
            fwd(SEQ[kk*nbs_arg_size_1+kk], SEQ[kk*nbs_arg_size_1+jj]);
         }
      }
      for (ii=kk+1; ii<nbs_arg_size_1; ii++) 
      {
         if (SEQ[ii*nbs_arg_size_1+kk] != NULL)
         {
            bdiv (SEQ[kk*nbs_arg_size_1+kk], SEQ[ii*nbs_arg_size_1+kk]);
         }
      }
      for (ii=kk+1; ii<nbs_arg_size_1; ii++)
      {
         if (SEQ[ii*nbs_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<nbs_arg_size_1; jj++)
               if (SEQ[kk*nbs_arg_size_1+jj] != NULL)
               {
                  if (SEQ[ii*nbs_arg_size_1+jj]==NULL) SEQ[ii*nbs_arg_size_1+jj] = allocate_clean_block();
                  bmod(SEQ[ii*nbs_arg_size_1+kk], SEQ[kk*nbs_arg_size_1+jj], SEQ[ii*nbs_arg_size_1+jj]);
               }
      }
   }  
   end = nbs_usecs();
   time = ((double)(end-start))/1000000;
   if (nbs_verbose_mode) print_structure("sequential", SEQ);
   return time;
}
double sparselu_seq_call(float ***pBENCH)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **BENCH = (float **) malloc(nbs_arg_size_1*nbs_arg_size_1*sizeof(float *));
   *pBENCH = BENCH;

   genmat(BENCH);
   if (nbs_verbose_mode) print_structure("sequential", BENCH);
   start = nbs_usecs();
   for (kk=0; kk<nbs_arg_size_1; kk++) 
   {
      lu0(BENCH[kk*nbs_arg_size_1+kk]);
      for (jj=kk+1; jj<nbs_arg_size_1; jj++)
         if (BENCH[kk*nbs_arg_size_1+jj] != NULL)
         {
            fwd(BENCH[kk*nbs_arg_size_1+kk], BENCH[kk*nbs_arg_size_1+jj]);
         }
      for (ii=kk+1; ii<nbs_arg_size_1; ii++) 
         if (BENCH[ii*nbs_arg_size_1+kk] != NULL)
         {
            bdiv (BENCH[kk*nbs_arg_size_1+kk], BENCH[ii*nbs_arg_size_1+kk]);
         }
      for (ii=kk+1; ii<nbs_arg_size_1; ii++)
         if (BENCH[ii*nbs_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<nbs_arg_size_1; jj++)
               if (BENCH[kk*nbs_arg_size_1+jj] != NULL)
               {
                     if (BENCH[ii*nbs_arg_size_1+jj]==NULL) BENCH[ii*nbs_arg_size_1+jj] = allocate_clean_block();
                     bmod(BENCH[ii*nbs_arg_size_1+kk], BENCH[kk*nbs_arg_size_1+jj], BENCH[ii*nbs_arg_size_1+jj]);
               }

   }

   end = nbs_usecs();
   time = ((double)(end-start))/1000000;
   if (nbs_verbose_mode) print_structure("sequential", BENCH);
   return time;
}
double sparselu_par_call(float ***pBENCH)
{
   int ii, jj, kk;
   long start, end;
   double time;
   float **BENCH = (float **) malloc(nbs_arg_size_1*nbs_arg_size_1*sizeof(float *));
   *pBENCH = BENCH;

   genmat(BENCH);
   if (nbs_verbose_mode) print_structure("benchmark", BENCH);
   start = nbs_usecs();
#pragma omp parallel
#pragma omp single
#pragma omp task untied
   {
   for (kk=0; kk<nbs_arg_size_1; kk++) 
   {
      lu0(BENCH[kk*nbs_arg_size_1+kk]);
      for (jj=kk+1; jj<nbs_arg_size_1; jj++)
         if (BENCH[kk*nbs_arg_size_1+jj] != NULL)
            #pragma omp task untied firstprivate(kk, jj) shared(BENCH)
         {
            fwd(BENCH[kk*nbs_arg_size_1+kk], BENCH[kk*nbs_arg_size_1+jj]);
         }
      for (ii=kk+1; ii<nbs_arg_size_1; ii++) 
         if (BENCH[ii*nbs_arg_size_1+kk] != NULL)
            #pragma omp task untied firstprivate(kk, ii) shared(BENCH)
         {
            bdiv (BENCH[kk*nbs_arg_size_1+kk], BENCH[ii*nbs_arg_size_1+kk]);
         }

      #pragma omp taskwait

      for (ii=kk+1; ii<nbs_arg_size_1; ii++)
         if (BENCH[ii*nbs_arg_size_1+kk] != NULL)
            for (jj=kk+1; jj<nbs_arg_size_1; jj++)
               if (BENCH[kk*nbs_arg_size_1+jj] != NULL)
               #pragma omp task untied firstprivate(kk, jj, ii) shared(BENCH)
               {
                     if (BENCH[ii*nbs_arg_size_1+jj]==NULL) BENCH[ii*nbs_arg_size_1+jj] = allocate_clean_block();
                     bmod(BENCH[ii*nbs_arg_size_1+kk], BENCH[kk*nbs_arg_size_1+jj], BENCH[ii*nbs_arg_size_1+jj]);
               }

      #pragma omp taskwait
   }
   }

   end = nbs_usecs();
   time = ((double)(end-start))/1000000;
   if (nbs_verbose_mode) print_structure("benchmark", BENCH);
   return time;
}
int sparselu_check(float **SEQ, float **BENCH)
{
   int ii,jj,ok=1;

   for (ii=0; ((ii<nbs_arg_size_1) && ok); ii++)
   {
      for (jj=0; ((jj<nbs_arg_size_1) && ok); jj++)
      {
         if ((SEQ[ii*nbs_arg_size_1+jj] == NULL) && (BENCH[ii*nbs_arg_size_1+jj] != NULL)) ok = FALSE;
         if ((SEQ[ii*nbs_arg_size_1+jj] != NULL) && (BENCH[ii*nbs_arg_size_1+jj] == NULL)) ok = FALSE;
         if ((SEQ[ii*nbs_arg_size_1+jj] != NULL) && (BENCH[ii*nbs_arg_size_1+jj] != NULL))
            ok = checkmat(SEQ[ii*nbs_arg_size_1+jj], BENCH[ii*nbs_arg_size_1+jj]);
      }
   }
   if (ok) return NBS_RESULT_SUCCESSFUL;
   else return NBS_RESULT_UNSUCCESSFUL;
}

