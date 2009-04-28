#include "omp-tasks-app.h"

#define BOTS_APP_NAME "N Queens"

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 14
#define BOTS_APP_DESC_ARG_SIZE "Board size"

int verify_queens(int);
void find_queens (int);
void find_queens_ser (int);

#define KERNEL_CALL find_queens(bots_arg_size)
#define KERNEL_SEQ_CALL find_queens_ser(bots_arg_size)
#define KERNEL_CHECK verify_queens(bots_arg_size)

#define BOTS_CUTOFF_DEF_VALUE 3
