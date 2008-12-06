#include "omp-tasks-app.h"

#define NBS_APP_NAME "SparseLU"
#define NBS_APP_PARAMETERS_DESC "S1=%dx%d, S2=%dx%d"
#define NBS_APP_PARAMETERS_LIST ,nbs_arg_size_1,nbs_arg_size_1,nbs_arg_size_2,nbs_arg_size_2

#define NBS_APP_CHECKING_NEEDS_SEQ
#define NBS_APP_CHECK_USES_SEQ_RESULT
#define NBS_APP_SELF_TIMING

#define NBS_APP_USES_ARG_SIZE_1
#define NBS_APP_DEF_ARG_SIZE_1 50
#define NBS_APP_DESC_ARG_SIZE_1 "Matrix Size"

#define NBS_APP_USES_ARG_SIZE_2
#define NBS_APP_DEF_ARG_SIZE_2 100
#define NBS_APP_DESC_ARG_SIZE_2 "Submatrix Size"

#define NBS_APP_INIT float **SEQ,**BENCH;

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

