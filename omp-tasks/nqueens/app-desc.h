#include "omp-tasks-app.h"

#define NBS_APP_NAME "N Queens"

#define NBS_APP_USES_ARG_SIZE
#define NBS_APP_DEF_ARG_SIZE 14
#define NBS_APP_DESC_ARG_SIZE "Board size"

int verify_queens(int);
void find_queens (int);
void find_queens_ser (int);

#define KERNEL_CALL find_queens(nbs_arg_size)
#define KERNEL_SEQ_CALL find_queens_ser(nbs_arg_size)
#define KERNEL_CHECK verify_queens(nbs_arg_size)

#define NBS_CUTOFF_DEF_VALUE 3
