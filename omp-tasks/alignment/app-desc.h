#include "omp-tasks-app.h"

#define NBS_APP_NAME "Protein alignment"
#define NBS_APP_PARAMETERS_DESC "%s"
#define NBS_APP_PARAMETERS_LIST ,nbs_arg_file

#define NBS_APP_USES_ARG_FILE
#define NBS_APP_DESC_ARG_FILE "Protein sequences file (mandatory)"

void pairalign_init(char *);
void align_init ();
void align ();
void align_end ();
void align_seq_init ();
void align_seq ();
int align_verify ();


#define NBS_APP_INIT pairalign_init(nbs_arg_file)

#define KERNEL_INIT align_init()
#define KERNEL_CALL align()
#define KERNEL_FINI align_end()

#define KERNEL_SEQ_INIT align_seq_init()
#define KERNEL_SEQ_CALL align_seq()
//#define KERNEL_SEQ_FINI

#define KERNEL_CHECK align_verify()
#define NBS_APP_CHECK_USES_SEQ_RESULT

