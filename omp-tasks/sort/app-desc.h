#include "omp-tasks-app.h"

#define BOTS_APP_NAME "Sort"
#define BOTS_APP_PARAMETERS_DESC "%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE (32*1024*1024)
#define BOTS_APP_DESC_ARG_SIZE "Array size"

int sort_verify(int);
void sort (int);
void sort_init (int);

#define KERNEL_INIT sort_init(bots_arg_size)
#define KERNEL_CALL sort(bots_arg_size)
#define KERNEL_CHECK sort_verify(bots_arg_size)


