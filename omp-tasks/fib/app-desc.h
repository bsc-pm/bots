#include "omp-tasks-app.h"

#define BOTS_APP_NAME "Fibonacci"

//#define BOTS_APP_SELF_TIMING
#define BOTS_APP_CHECKING_NEEDS_SEQ

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 10
#define BOTS_APP_DESC_ARG_SIZE "Number to compute"

int fib_verify();
void fib0 (int);
void fib0_seq (int);

//#define KERNEL_INIT
#define KERNEL_CALL fib0(bots_arg_size)
//#define KERNEL_FINI

//#define KERNEL_SEQ_INIT
#define KERNEL_SEQ_CALL fib0_seq(bots_arg_size)
//#define KERNEL_SEQ_FINI


#define KERNEL_CHECK fib_verify()
#define BOTS_APP_CHECK_USES_SEQ_RESULT

#define BOTS_CUTOFF_DEF_VALUE 4
