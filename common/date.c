#include <stdio.h>
#include "common.h"

/***********************************************************************
 * DEFAULT VALUES
 *********************************************************************/
/***********************************************************************
 * DEFAULT VALUES
 *********************************************************************/
/* common flags */
int bsc_benchmark = TRUE;
int bsc_sequential = FALSE;
int bsc_check = FALSE;
int bsc_row_output = FALSE;
int bsc_verbose = FALSE;
/* common variables */
char bsc_program[128];
char bsc_parameters[128];
char bsc_model[128];
char bsc_resources[128];
double bsc_time_benchmark = 0.0;
double bsc_time_sequential = 0.0;
int bsc_benchmark_ok = TRUE;
char bsc_exec_message[128];
char bsc_comp_date[128];
char bsc_comp_message[128];
char bsc_cc[128];
char bsc_cflags[128];
char bsc_ld[128];
char bsc_ldflags[128];

int main(){
  char date[32];
  bsc_get_date(date);
  fprintf(stdout, "%s\n",date);
  return 0;
}
  
