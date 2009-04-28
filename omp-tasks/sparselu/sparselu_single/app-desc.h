#include "omp-tasks-app.h"

#define BOTS_APP_NAME "SparseLU"
#define BOTS_APP_PARAMETERS_DESC "S1=%dx%d, S2=%dx%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size_1,bots_arg_size_1,bots_arg_size_2,bots_arg_size_2

#define BOTS_APP_CHECKING_NEEDS_SEQ
#define BOTS_APP_CHECK_USES_SEQ_RESULT
#define BOTS_APP_SELF_TIMING

#define BOTS_APP_USES_ARG_SIZE_1
#define BOTS_APP_DEF_ARG_SIZE_1 50
#define BOTS_APP_DESC_ARG_SIZE_1 "Matrix Size"

#define BOTS_APP_USES_ARG_SIZE_2
#define BOTS_APP_DEF_ARG_SIZE_2 100
#define BOTS_APP_DESC_ARG_SIZE_2 "Submatrix Size"

#define BOTS_APP_INIT float **SEQ,**BENCH;

int checkmat (float *M, float *N);
void genmat (float *M[]);
void print_structure(char *name, float *M[]);
float * allocate_clean_block();
void lu0(float *diag);
void bdiv(float *diag, float *row);
void bmod(float *row, float *col, float *inner);
void fwd(float *diag, float *col);
double sparselu_seq_call(float ***SEQ);
double sparselu_par_call(float ***BENCH);
int sparselu_check(float **SEQ, float **BENCH);

#define KERNEL_INIT
#define KERNEL_CALL sparselu_par_call(&BENCH);
#define KERNEL_FINI

#define KERNEL_SEQ_INIT
#define KERNEL_SEQ_CALL sparselu_seq_call(&SEQ);
#define KERNEL_SEQ_FINI

#define KERNEL_CHECK sparselu_check(SEQ,BENCH);

