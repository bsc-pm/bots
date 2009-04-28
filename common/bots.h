/***********************************************************************
 * GLOBAL SYMBOLS 
 *********************************************************************/
/* common flags */
extern int bots_sequential_flag;
extern int bots_check_flag;
extern int bots_result;
extern int bots_output_format;
extern int bots_print_header;
/* common variables */
extern char bots_name[];
extern char bots_parameters[];
extern char bots_model[];
extern char bots_resources[];
/* compile and execution information */
extern char bots_exec_date[];
extern char bots_exec_message[];
extern char bots_comp_date[];
extern char bots_comp_message[];
extern char bots_cc[];
extern char bots_cflags[];
extern char bots_ld[];
extern char bots_ldflags[];
/* time variables */
extern double bots_time_program;
extern double bots_time_sequential;
extern int    bots_number_of_tasks;

extern char bots_cutoff[];
extern int  bots_cutoff_value;
extern int  bots_verbose_mode;

extern int bots_arg_size;
extern int bots_arg_size_1;
extern int bots_arg_size_2;

/* function could be used in app. code */
long bots_usecs();

#define BOTS_RESULT_NA 0
#define BOTS_RESULT_SUCCESSFUL 1
#define BOTS_RESULT_UNSUCCESSFUL 2

#define FALSE 0
#define TRUE 1

