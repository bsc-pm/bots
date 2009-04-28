#include "omp-tasks-app.h"

#define BOTS_APP_NAME "Knapsack"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file

#define BOTS_APP_CHECK_USES_SEQ_RESULT

//#define BOTS_APP_SELF_TIMING

#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DEF_ARG_FILE "Input filename"
#define BOTS_APP_DESC_ARG_FILE

#define BOTS_CUTOFF_DEF_VALUE 24

/* every item in the knapsack has a weight and a value */
#define MAX_ITEMS 256


struct item {
     int value;
     int weight;
};

int compare(struct item *a, struct item *b);
int read_input(const char *filename, struct item *items, int *capacity, int *n);
void knapsack_seq(struct item *e, int c, int n, int v, int *sol);
void knapsack_par(struct item *e, int c, int n, int v, int *sol, int l);
void knapsack_main_seq (struct item *e, int c, int n, int v, int *sol);
void knapsack_main_par (struct item *e, int c, int n, int v, int *sol);
int  knapsack_check (int sol_seq, int sol_par);

#define BOTS_APP_INIT\
     struct item items[MAX_ITEMS];\
     int n, capacity;\
     int sol_seq = 0, sol_par = 0;\
     read_input(bots_arg_file, items, &capacity, &n);

#define KERNEL_INIT 
#define KERNEL_CALL knapsack_main_par(items, capacity, n, 0, &sol_par);
#define KERNEL_FINI

#define KERNEL_SEQ_INIT 
#define KERNEL_SEQ_CALL knapsack_main_seq(items, capacity, n, 0, &sol_seq);
#define KERNEL_SEQ_FINI

#define KERNEL_CHECK  knapsack_check(sol_seq, sol_par)

