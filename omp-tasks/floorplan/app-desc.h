#include "omp-tasks-app.h"

#define NBS_APP_NAME "Floorplan"

#define NBS_APP_USES_ARG_FILE
#define NBS_APP_DESC_ARG_FILE "Cell description file (mandatory)"

void floorplan_init(char *);
void floorplan_end();
void compute_floorplan();

#define KERNEL_INIT floorplan_init(nbs_arg_file)
#define KERNEL_CALL compute_floorplan()
#define KERNEL_FINI floorplan_end()


