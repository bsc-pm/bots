#include "omp-tasks-app.h"

#define BOTS_APP_NAME "Floorplan"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file

#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DESC_ARG_FILE "Cell description file (mandatory)"

#define BOTS_CUTOFF_DEF_VALUE 5

void floorplan_init(char *);
void floorplan_end (void);
void compute_floorplan(void);
int floorplan_verify(void);

#define KERNEL_INIT floorplan_init(bots_arg_file)
#define KERNEL_CALL compute_floorplan()
#define KERNEL_FINI floorplan_end()

#define KERNEL_CHECK floorplan_verify()


