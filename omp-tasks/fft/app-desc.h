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

#include "omp-tasks-app.h"

#define BOTS_APP_NAME "FFT"
#define BOTS_APP_PARAMETERS_DESC "Size=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size

#define BOTS_APP_CHECKING_NEEDS_SEQ

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 32*1024*1024
#define BOTS_APP_DESC_ARG_SIZE "Matrix Size"

#define BOTS_APP_CHECK_USES_SEQ_RESULT

/* our real numbers */
typedef float REAL;

/* Complex numbers and operations */
typedef struct {
     REAL re, im;
} COMPLEX;

#define c_re(c)  ((c).re)
#define c_im(c)  ((c).im)

void compute_w_coefficients(int n, int a, int b, COMPLEX * W);
void compute_w_coefficients_seq(int n, int a, int b, COMPLEX * W);
int factor(int n);
void unshuffle(int a, int b, COMPLEX * in, COMPLEX * out, int r, int m);
void unshuffle_seq(int a, int b, COMPLEX * in, COMPLEX * out, int r, int m);
void fft_twiddle_gen1(COMPLEX * in, COMPLEX * out, COMPLEX * W, int r, int m, int nW, int nWdnti, int nWdntm);
void fft_twiddle_gen(int i, int i1, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int r, int m);
void fft_twiddle_gen_seq(int i, int i1, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int r, int m);
void fft_base_2(COMPLEX * in, COMPLEX * out);
void fft_twiddle_2(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_twiddle_2_seq(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_unshuffle_2(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_unshuffle_2_seq(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_base_4(COMPLEX * in, COMPLEX * out);
void fft_twiddle_4(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_twiddle_4_seq(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_unshuffle_4(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_unshuffle_4_seq(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_base_8(COMPLEX * in, COMPLEX * out);
void fft_twiddle_8(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_twiddle_8_seq(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_unshuffle_8(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_unshuffle_8_seq(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_base_16(COMPLEX * in, COMPLEX * out);
void fft_twiddle_16(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_twiddle_16_seq(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_unshuffle_16(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_unshuffle_16_seq(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_base_32(COMPLEX * in, COMPLEX * out);
void fft_twiddle_32(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_twiddle_32_seq(int a, int b, COMPLEX * in, COMPLEX * out, COMPLEX * W, int nW, int nWdn, int m);
void fft_unshuffle_32(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_unshuffle_32_seq(int a, int b, COMPLEX * in, COMPLEX * out, int m);
void fft_aux(int n, COMPLEX * in, COMPLEX * out, int *factors, COMPLEX * W, int nW);
void fft_aux_seq(int n, COMPLEX * in, COMPLEX * out, int *factors, COMPLEX * W, int nW);
void fft(int n, COMPLEX * in, COMPLEX * out);
void fft_seq(int n, COMPLEX * in, COMPLEX * out);
int test_correctness(int n, COMPLEX *out1, COMPLEX *out2);

#define BOTS_APP_INIT int i;\
     COMPLEX *in, *out1, *out2;\
     in = malloc(bots_arg_size * sizeof(COMPLEX));\

#define KERNEL_INIT\
     out1 = malloc(bots_arg_size * sizeof(COMPLEX));\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_CALL fft(bots_arg_size, in, out1);
#define KERNEL_FINI 

#define KERNEL_SEQ_INIT\
     out2 = malloc(bots_arg_size * sizeof(COMPLEX));\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_SEQ_CALL fft_seq(bots_arg_size, in, out2);
#define KERNEL_SEQ_FINI


#define KERNEL_CHECK test_correctness(bots_arg_size, out1, out2)

