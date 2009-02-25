#include "omp-tasks-app.h"

#define NBS_APP_NAME "Floorplan"

#define NBS_APP_USES_ARG_FILE
#define NBS_APP_DESC_ARG_FILE "Cell description file (mandatory)"

#define NBS_CUTOFF_DEF_VALUE 5

void floorplan_init(char *);
void floorplan_end (void);
void compute_floorplan(void);
int floorplan_verify(void);

#define KERNEL_INIT floorplan_init(nbs_arg_file)
#define KERNEL_CALL compute_floorplan()
#define KERNEL_FINI floorplan_end()

#define KERNEL_CHECK floorplan_verify()


