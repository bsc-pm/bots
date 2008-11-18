/***********************************************************************
 * template.c -- description
 **********************************************************************/
#include "../../common/common.h"
#include "../../common/extern.h"
#include "../../common/main.h"
/**********************************************************************/
#include <stdio.h>
#include <string.h>

/***********************************************************************
 * GLOBAL SYMBOLS & DEFAULT VALUES 
 *********************************************************************/

/***********************************************************************
 * function:
 **********************************************************************/

/***********************************************************************
 ***********************************************************************
 * bsc_sequential: 
 ***********************************************************************
 **********************************************************************/
/***********************************************************************
 * bsc_sequential_ini: Executed just after the sequential function 
 **********************************************************************/
void bsc_sequential_ini ()
{
}
/***********************************************************************
 * bsc_sequential: Equivalent sequential version of the program 
 ***********************************************************************
 * If the function returns (0) main take cares about execution
 * time measurement. Otherwise the returned value will be used
 * as execution time reference (in usecs). You can use bsc_usecs()
 * function in order to get punctual timestamp.
 **********************************************************************/
long bsc_sequential ()
{
   return 0;
}
/***********************************************************************
 * bsc_sequential_fini: Executed just before the sequential function 
 **********************************************************************/
void bsc_sequential_fini ()
{
}
/***********************************************************************
 ***********************************************************************
 * bsc_program *********************************************************
 ***********************************************************************
 **********************************************************************/
/***********************************************************************
 * bsc_program_ini: Executed just after the program function 
 **********************************************************************/
void bsc_program_ini ()
{
}
/***********************************************************************
 * bsc_program: Program executed code (according with programmin model) 
 ***********************************************************************
 * If the function returns (0) main take cares about execution
 * time measurement. Otherwise the returned value will be used
 * as execution time reference (in usecs). You can use bsc_usecs()
 * function in order to get punctual timestamp.
 **********************************************************************/
long bsc_program ()
{
   return 0;
}
/***********************************************************************
 * bsc_program_fini: Executed just before the program function 
 **********************************************************************/
void bsc_program_fini ()
{
}
/***********************************************************************
 * bsc_initialize: Executed at the beginning of the benchmark 
 **********************************************************************/
void bsc_initialize ()
{
}
/***********************************************************************
 * bsc_finalize: Executed at the end of the benchmark
 **********************************************************************/
void bsc_finalize ()
{
}
/***********************************************************************
 * bsc_check_result: Performs a verification of correctness.
 ***********************************************************************
 * Returns:
 *    - BSC_RESULT_NA (not available)
 *    - BSC_RESULT_SUCCESSFUL
 *    - BSC_RESULT_UNSUCCESSFUL 
 **********************************************************************/
int
bsc_check_result ()
{
   int check_result = BSC_RESULT_NA;

   return check_result; 
}
/***********************************************************************
 * bsc_print_usage_specific: Show some specific help
 ***********************************************************************
 * To keep output format use the function bsc_print_usage_option().
 * Parameters are (in order):
 *    char: flag identifier
 *    int: argument type
 *          - BSC_PARAM_TYPE_NONE: without argument
 *          - BSC_PARAM_TYPE_INT: integer argument
 *          - BSC_PARAM_TYPE_STR: string argument
 *          - BSC_PARAM_TYPE_BOOL: boolean argument
 *    char*: parameter desccription
 *    char*: argument default value (NULL if none)
 *    int: number of aditional description lines (dl)
 *    char**: aditional description lines
 **********************************************************************/
void bsc_print_usage_specific ()
{
}
/***********************************************************************
 * bsc_get_params_specific: Capture program paramenters 
 **********************************************************************/
void bsc_get_params_specific (int argc, char **argv)
{
   int i;

   for (i=1; i<argc; i++) 
   {
      if (argv[i][0] == '-')
      {
         switch (argv[i][1])
         {
#if 0
            case ' ': /* parameter description */
               i++; /* read argument (if needed) */
               if (argc == i){bsc_error(BSC_ERROR_UNRECOGNIZED_PARAMETER, NULL);}
               INT_GOBAL_VALUE = atoi(argv[i]);
               BOOL_GOBAL_VALUE = atoi(argv[i]);
               strcpy(STR_GLOBAL_VALUE,argv[i]);
               break;
#endif
            case '+': /* already proccessed paramenters no-argument */
               break;
            case '*': /* already proccessed paramenters w/-argument */
               i++;
               break;
            default: /* unrecognized parameter: error */
               bsc_error(BSC_ERROR_UNRECOGNIZED_PARAMETER, NULL);
         }
      }
      else {bsc_error(BSC_ERROR_UNRECOGNIZED_PARAMETER, NULL);}
   }
}
/***********************************************************************
 * bsc_set_info: Initialize some global (extern) variables
 ***********************************************************************
 * Global variables to initialize:
 *    - bsc_paramenters: executed paramenters separated by comma
 *    - bsc_model: programming model
 *    - bsc_resources: machine resources (e.g. number of threads)
 *    - bsc_comp_date: compilation date
 *    - bsc_comp_message: user's message at compile time
 *    - bsc_cc: compiler name
 *    - bsc_cflags: flags used by the compiler
 *    - bsc_ld: linker name
 *    - bsc_ldflags: flags used by the linker
 **********************************************************************/
void bsc_set_info ()
{
   /* program specific info */
   sprintf(bsc_parameters,"par1,par2,...");
   sprintf(bsc_model,"Programming-Model");
   sprintf(bsc_resources,"%d", omp_get_max_threads());
   /* compilation info (do not modify) */
   strcpy(bsc_comp_date,CDATE);
   strcpy(bsc_comp_message,CMESSAGE);
   strcpy(bsc_cc,CC);
   strcpy(bsc_cflags,CFLAGS);
   strcpy(bsc_ld,LD);
   strcpy(bsc_ldflags,LDFLAGS);
}

