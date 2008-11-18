#include "omp-tasks-app.h"

#define NBS_APP_NAME "Strassen"
#define NBS_APP_PARAMETERS_DESC "N=%d"
#define NBS_APP_PARAMETERS_LIST ,nbs_arg_size_1

#define NBS_APP_CHECKING_NEEDS_SEQ

#define NBS_APP_USES_ARG_SIZE_1
#define NBS_APP_DEF_ARG_SIZE_1 1024
#define NBS_APP_DESC_ARG_SIZE_1 "Matrix Size"

#define NBS_APP_USES_ARG_BLOCK
#define NBS_APP_DEF_ARG_BLOCK 32
#define NBS_APP_DESC_ARG_BLOCK "Matrix Block Size"

#define NBS_APP_USES_ARG_CUTOFF
#define NBS_APP_DEF_ARG_CUTOFF 64
#define NBS_APP_DESC_ARG_CUTOFF "Strassen Cutoff"

#define NBS_CUTOFF_DEF_VALUE 3

/***********************************************************************
 * The real numbers we are using --- either double or float
 **********************************************************************/
typedef double REAL;
typedef unsigned long PTR;

void matrixmul(int n, REAL *A, int an, REAL *B, int bn, REAL *C, int cn);
void FastNaiveMatrixMultiply(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB);
void FastAdditiveNaiveMatrixMultiply(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB);
void MultiplyByDivideAndConquer(REAL *C, REAL *A, REAL *B,
				     unsigned MatrixSize,
				     unsigned RowWidthC,
				     unsigned RowWidthA,
				     unsigned RowWidthB,
				     int AdditiveMode
				    );
void OptimizedStrassenMultiply_par(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB, int Depth);
void OptimizedStrassenMultiply_seq(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB, int Depth);
void init_matrix(int n, REAL *A, int an);
int compare_matrix(int n, REAL *A, int an, REAL *B, int bn);
REAL *alloc_matrix(int n);
void strassen_main_par(REAL *A, REAL *B, REAL *C, int n);
void strassen_main_seq(REAL *A, REAL *B, REAL *C, int n);

#define NBS_APP_INIT\
    double *A, *B, *C, *D;\
    if ((nbs_arg_size_1 & (nbs_arg_size_1 - 1)) != 0 || (nbs_arg_size_1 % 16) != 0) {\
        printf("Error: matrix size (%d) must be a power of 2 and a multiple of %d\n", nbs_arg_size_1, 16);\
        exit (1);\
    }\
    A = (double *) malloc (nbs_arg_size_1 * nbs_arg_size_1 * sizeof(double));\
    B = (double *) malloc (nbs_arg_size_1 * nbs_arg_size_1 * sizeof(double));\
    C = (double *) malloc (nbs_arg_size_1 * nbs_arg_size_1 * sizeof(double));\
    D = (double *) malloc (nbs_arg_size_1 * nbs_arg_size_1 * sizeof(double));\
    init_matrix(nbs_arg_size_1,A,nbs_arg_size_1);\
    init_matrix(nbs_arg_size_1,B,nbs_arg_size_1);

//#define KERNEL_INIT
#define KERNEL_CALL strassen_main_par(C,A,B,nbs_arg_size_1);
//#define KERNEL_FINI

//#define KERNEL_SEQ_INIT
#define KERNEL_SEQ_CALL strassen_main_seq(D,A,B,nbs_arg_size_1);
//#define KERNEL_SEQ_FINI


#define KERNEL_CHECK compare_matrix(nbs_arg_size_1,C,nbs_arg_size_1,D,nbs_arg_size_1);

#define NBS_APP_CHECK_USES_SEQ_RESULT

