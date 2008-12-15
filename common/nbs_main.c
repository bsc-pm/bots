/***********************************************************************
 * main function & common behaviour of the benchmark.
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <memory.h>
#include <sys/time.h>
#include <libgen.h>
#include "nbs_common.h"
#include "nbs_main.h"
#include "nbs.h"
#include "app-desc.h"

/***********************************************************************
 * BENCHMARK HEADERS 
 *********************************************************************/
void nbs_initialize();
void nbs_finalize();
void nbs_sequential_ini();
long nbs_sequential();
void nbs_sequential_fini();
int nbs_check_result();
void nbs_print_usage_specific();
void nbs_get_params_specific(int argc, char **argv);
void nbs_set_info();

/***********************************************************************
 * DEFAULT VALUES 
 *********************************************************************/
/* common flags */
int nbs_sequential_flag = FALSE;
int nbs_check_flag = FALSE;
int nbs_verbose_mode = 0;
int nbs_result = NBS_RESULT_NA;
int nbs_output_format = 0;
int nbs_print_header = FALSE;
/* common variables */
char nbs_name[128];
char nbs_execname[128];
char nbs_parameters[128];
char nbs_model[128];
char nbs_resources[128];
/* compile and execution information */
char nbs_exec_date[128];
char nbs_exec_message[128];
char nbs_comp_date[128];
char nbs_comp_message[128];
char nbs_cc[128];
char nbs_cflags[128];
char nbs_ld[128];
char nbs_ldflags[128];
char nbs_cutoff[128];

/* time variables */
double nbs_time_program = 0.0;
double nbs_time_sequential = 0.0;

#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
int  nbs_cutoff_value=NBS_CUTOFF_DEF_VALUE;
#endif

/*
 * Application dependent info
 */

#ifndef NBS_APP_NAME
#error "Application name must be defined (#define NBS_APP_NAME)"
#endif

#ifndef NBS_APP_PARAMETERS_DESC
#define NBS_APP_PARAMETERS_DESC ""
#endif

#ifndef NBS_APP_PARAMETERS_LIST
#define NBS_APP_PARAMETERS_LIST
#endif

#ifndef NBS_APP_INIT
#define NBS_APP_INIT
#endif

#ifndef NBS_APP_FINI
#define NBS_APP_FINI
#endif

#ifndef KERNEL_CALL
#error "Initial kernell call must be specified (#define KERNEL_CALL)"
#endif

#ifndef KERNEL_INIT
#define KERNEL_INIT
#endif

#ifndef KERNEL_FINI
#define KERNEL_FINI
#endif

#ifndef KERNEL_SEQ_INIT
#define KERNEL_SEQ_INIT
#endif

#ifndef KERNEL_SEQ_FINI
#define KERNEL_SEQ_FINI
#endif

#ifndef NBS_MODEL_DESC
#define NBS_MODEL_DESC "Unknown"
#endif

/* FIXME: remove arg size to arg_size_0, use only arg_size_1 and arg_size_2 */
#ifdef NBS_APP_USES_ARG_SIZE
#ifndef NBS_APP_DEF_ARG_SIZE
#error "Default vaule for argument size must be specified (#define NBS_APP_DEF_ARG_SIZE)"
#endif
#ifndef NBS_APP_DESC_ARG_SIZE
#error "Help description for argument size must be specified (#define NBS_APP_DESC_ARG_SIZE)"
#endif
int nbs_arg_size = NBS_APP_DEF_ARG_SIZE;
#endif

#ifdef NBS_APP_USES_ARG_SIZE_1
#ifndef NBS_APP_DEF_ARG_SIZE_1
#error "Default vaule for argument size must be specified (#define NBS_APP_DEF_ARG_SIZE_1)"
#endif
#ifndef NBS_APP_DESC_ARG_SIZE_1
#error "Help description for argument size must be specified (#define NBS_APP_DESC_ARG_SIZE_1)"
#endif
int nbs_arg_size_1 = NBS_APP_DEF_ARG_SIZE_1;
#endif

#ifdef NBS_APP_USES_ARG_SIZE_2
#ifndef NBS_APP_DEF_ARG_SIZE_2
#error "Default vaule for argument size must be specified (#define NBS_APP_DEF_ARG_SIZE_2)"
#endif
#ifndef NBS_APP_DESC_ARG_SIZE_2
#error "Help description for argument size must be specified (#define NBS_APP_DESC_ARG_SIZE_2)"
#endif
int nbs_arg_size_2 = NBS_APP_DEF_ARG_SIZE_2;
#endif

#ifdef NBS_APP_USES_ARG_REPETITIONS
#ifndef NBS_APP_DEF_ARG_REPETITIONS
#error "Default vaule for argument repetitions must be specified (#define NBS_APP_DEF_ARG_REPETITIONS)"
#endif
#ifndef NBS_APP_DESC_ARG_REPETITIONS
#error "Help description for argument repetitions must be specified (#define NBS_APP_DESC_ARG_REPETITIONS)"
#endif
int nbs_arg_repetitions = NBS_APP_DEF_ARG_REPETITIONS;
#endif

#ifdef NBS_APP_USES_ARG_FILE
#ifndef NBS_APP_DESC_ARG_FILE
#error "Help description for argument file must be specified (#define NBS_APP_DESC_ARG_FILE)"
#endif
char nbs_arg_file[255]="";
#endif

#ifdef NBS_APP_USES_ARG_BLOCK
#ifndef NBS_APP_DEF_ARG_BLOCK
#error "Default value for argument block must be specified (#define NBS_APP_DEF_ARG_BLOCK)"
#endif
#ifndef NBS_APP_DESC_ARG_BLOCK
#error "Help description for argument block must be specified (#define NBS_APP_DESC_ARG_BLOCK)"
#endif

int nbs_arg_block = NBS_APP_DEF_ARG_BLOCK;
#endif

#ifdef NBS_APP_USES_ARG_CUTOFF
#ifndef NBS_APP_DEF_ARG_CUTOFF
#error "Default value for argument cutoff  must be specified (#define NBS_APP_DEF_ARG_CUTOFF)"
#endif
#ifndef NBS_APP_DESC_ARG_CUTOFF
#error "Help description for argument cutoff must be specified (#define NBS_APP_DESC_ARG_CUTOFF)"
#endif
int nbs_arg_cutoff = NBS_APP_DEF_ARG_CUTOFF;
#endif

/***********************************************************************
 * print_usage: 
 **********************************************************************/
void nbs_print_usage()
{
   fprintf(stderr, "\n");
   fprintf(stderr, "Usage: %s -[options]\n", nbs_execname);
   fprintf(stderr, "\n");
   fprintf(stderr, "Where options are:\n");
#ifdef NBS_APP_USES_ARG_SIZE
   fprintf(stderr, "  -n <size>: "NBS_APP_DESC_ARG_SIZE"\n");
#endif
#ifdef NBS_APP_USES_ARG_SIZE
   fprintf(stderr, "  -m <size>: "NBS_APP_DESC_ARG_SIZE_1"\n");
#endif
#ifdef NBS_APP_USES_ARG_SIZE
   fprintf(stderr, "  -l <size>: "NBS_APP_DESC_ARG_SIZE_2"\n");
#endif
#ifdef NBS_APP_USES_ARG_FILE
   fprintf(stderr, "  -f <file>: "NBS_APP_DESC_ARG_FILE"\n");
#endif
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
   fprintf(stderr, "  -x <value>: cut-off value (default=%d)\n",NBS_CUTOFF_DEF_VALUE);
#endif
   fprintf(stderr, "\n");
   fprintf(stderr, "  -e <str>: Include 'str' execution message.\n");
   fprintf(stderr, "  -o <nnn>: Set output format mode (default = 0).\n");
   fprintf(stderr, "\n");
#ifdef KERNEL_SEQ_CALL
   fprintf(stderr, "  -s [1|0]: Set/Unset run sequential version (default = UNSET).\n");
#endif
   fprintf(stderr, "  -c [1|0]: Set/Unset check mode (default = UNSET).\n");
#ifdef NBS_APP_CHECK_USES_SEQ_RESULT
   fprintf(stderr, "            Implies running sequential version.\n");
#endif
   fprintf(stderr, "  -v <level>: Set verbose level (default = 0).\n");
   fprintf(stderr, "  -r [1|0]: Set/Unset print header (default = UNSET).\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  -h : Print program's usage (this help).\n");
   fprintf(stderr, "\n");
}
/***********************************************************************
 * nbs_get_params_common: 
 **********************************************************************/
void
nbs_get_params_common(int argc, char **argv)
{
   int i;
   strcpy(nbs_execname, basename(argv[0]));
   nbs_get_date(nbs_exec_date);
   strcpy(nbs_exec_message,"");
   for (i=1; i<argc; i++) 
   {
      if (argv[i][0] == '-')
      {
         switch (argv[i][1])
         {
            case 'e': /* include execution message */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               strcpy(nbs_exec_message, argv[i]);
               break;
            case 'c': /* set/unset check mode */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_check_flag = atoi(argv[i]);
               break;
#ifdef NBS_APP_USES_ARG_FILE
            case 'f': /* read argument size */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               strcpy(nbs_arg_file,argv[i]);
               break;
#endif
#ifdef NBS_APP_USES_ARG_SIZE
            case 'n': /* read argument size 0 */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_arg_size = atoi(argv[i]);
               break;
#endif
#ifdef NBS_APP_USES_ARG_SIZE_1
            case 'm': /* read argument size 1 */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_arg_size_1 = atoi(argv[i]);
               break;
#endif
#ifdef NBS_APP_USES_ARG_SIZE_2
            case 'l': /* read argument size 2 */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_arg_size_2 = atoi(argv[i]);
               break;
#endif
/*TODO*/
#ifdef NBS_APP_USES_ARG_BLOCK
#endif
            case 'o': /* set/unset row output mode */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_output_format = atoi(argv[i]);
               break;
#ifdef NBS_APP_USES_REPETITIONS
            case 'r': /* set number of repetitions */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_arg_repetition = atoi(argv[i]);
               break;
#endif
#ifdef KERNEL_SEQ_CALL
            case 's': /* set/unset sequential execution */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_sequential_flag = atoi(argv[i]);
               break;
#endif
            case 'v': /* set/unset verbose mode */
               argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_verbose_mode = atoi(argv[i]);
               break;
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF)
	    case 'x':
	       argv[i][1] = '*';
               i++;
               if (argc == i) { nbs_print_usage(); exit(100); }
               nbs_cutoff_value = atoi(argv[i]);
               break;
#endif
/*TODO*/
#ifdef NBS_APP_USES_ARG_CUTOFF
#endif
            case 'h': /* print usage */
               argv[i][1] = '*';
               nbs_print_usage();
               exit (100);
            default:
               i++;
               break;
         }
      }
      else
      {
         fprintf(stderr, "Error: Unrecognized parameter.\n");
         nbs_print_usage();
         exit (100);
      }
   }
}

/***********************************************************************
 * nbs_get_params_common: 
 **********************************************************************/
void
nbs_get_params(int argc, char **argv)
{
   nbs_get_params_common(argc, argv);
//   nbs_get_params_specific(argc, argv);
}


/***********************************************************************
 * nbs_set_info 
 **********************************************************************/
void nbs_set_info ()
{
   /* program specific info */
   sprintf(nbs_name,NBS_APP_NAME);
   sprintf(nbs_parameters,NBS_APP_PARAMETERS_DESC NBS_APP_PARAMETERS_LIST);
   sprintf(nbs_model,NBS_MODEL_DESC);

   sprintf(nbs_resources,"%d", omp_get_max_threads());
   /* compilation info (do not modify) */
   strcpy(nbs_comp_date,CDATE);
   strcpy(nbs_comp_message,CMESSAGE);
   strcpy(nbs_cc,CC);
   strcpy(nbs_cflags,CFLAGS);
   strcpy(nbs_ld,LD);
   strcpy(nbs_ldflags,LDFLAGS);

#if defined(MANUAL_CUTOFF) 
   sprintf(nbs_cutoff,"manual (%d)",nbs_cutoff_value);
#elif defined(IF_CUTOFF) 
   sprintf(nbs_cutoff,"pragma-if (%d)",nbs_cutoff_value);
#else
   strcpy(nbs_cutoff,"none");
#endif
}

/***********************************************************************
 * main: 
 **********************************************************************/
int
main(int argc, char* argv[])
{
#ifndef NBS_APP_SELF_TIMING
   long nbs_t_start;
   long nbs_t_end;
#endif

   nbs_get_params(argc,argv);
   nbs_set_info();

   NBS_APP_INIT;

#ifdef KERNEL_SEQ_CALL
#ifdef NBS_APP_CHECK_USES_SEQ_RESULT
   if (nbs_sequential_flag || nbs_check_flag)
#else
   if (nbs_sequential_flag)
#endif
   {
      nbs_sequential_flag = 1;
      KERNEL_SEQ_INIT;
#ifdef NBS_APP_SELF_TIMING
      nbs_time_sequential = KERNEL_SEQ_CALL;
#else
      nbs_t_start = nbs_usecs();
      KERNEL_SEQ_CALL;
      nbs_t_end = nbs_usecs();
      nbs_time_sequential = ((double)(nbs_t_end-nbs_t_start))/1000000;
#endif
      KERNEL_SEQ_FINI;
   }
#endif

   KERNEL_INIT;
#ifdef NBS_APP_SELF_TIMING
   nbs_time_program = KERNEL_CALL;
#else
   nbs_t_start = nbs_usecs();
   KERNEL_CALL;
   nbs_t_end = nbs_usecs();
   nbs_time_program = ((double)(nbs_t_end-nbs_t_start))/1000000;
#endif
   KERNEL_FINI;

#ifdef KERNEL_CHECK
   if (nbs_check_flag)
   {
      nbs_result = KERNEL_CHECK;
   }
#endif

   NBS_APP_FINI;

   nbs_print_results();
   return (0);
}

