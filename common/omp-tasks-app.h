#include <omp.h>

#define MODEL OMP-TASKS

#ifdef FORCE_TIED_TASKS
#define NBS_MODEL_DESC "OpenMP (using tied tasks)"
#else
#define NBS_MODEL_DESC "OpenMP (using tasks)"
#endif


