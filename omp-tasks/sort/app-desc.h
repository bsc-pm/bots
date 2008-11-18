#include "omp-tasks-app.h"

#define NBS_APP_NAME "Sort"

#define NBS_APP_USES_ARG_SIZE
#define NBS_APP_DEF_ARG_SIZE (32*1024*1024)
#define NBS_APP_DESC_ARG_SIZE "Array size"

int sort_verify(int);
void sort (int);
void sort_init (int);

#define KERNEL_INIT sort_init(nbs_arg_size)
#define KERNEL_CALL sort(nbs_arg_size)
#define KERNEL_CHECK sort_verify(nbs_arg_size)


