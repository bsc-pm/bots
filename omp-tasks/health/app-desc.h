#include "omp-tasks-app.h"

#define NBS_APP_NAME "Health"
#define NBS_APP_PARAMETERS_DESC "%s"
#define NBS_APP_PARAMETERS_LIST ,nbs_arg_file

//#define NBS_APP_SELF_TIMING

#define NBS_APP_USES_ARG_FILE
#define NBS_APP_DEF_ARG_FILE "Input filename"
#define NBS_APP_DESC_ARG_FILE

#define NBS_CUTOFF_DEF_VALUE 2

/* random defines */
#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

extern int sim_level;

struct Results {
   float hosps_number;
   float hosps_personnel;
   float total_patients;
   float total_in_village;
   float total_waiting;
   float total_assess;
   float total_inside;
   float total_time;
   float total_hosps_v;
};
struct Patient {
   int id;
   long seed;
   int time;
   int time_left;
   int hosps_visited;
   struct Village *home_village;
};
struct List {
   struct Patient *patient;
   struct List *back;
   struct List *forward;
};
struct Hosp {
   int personnel;
   int free_personnel;
   struct List waiting;
   struct List assess;
   struct List inside;
   struct List realloc;
   omp_lock_t  realloc_lock;
};
struct Village {
   int id;
   struct Village *back;
   struct Village *next;
   struct Village *forward;
   struct List population;
   struct Hosp hosp;
   int level;
   long  seed;
};

void read_input_data(char *filename);
float my_rand(long *seed);

void allocate_village( struct Village **capital, struct Village *back, struct Village *next, int level, int vid);

struct Patient *generate_patient(struct Village *village);
void put_in_hosp(struct Hosp *hosp, struct Patient *patient);

void addList(struct List *list, struct Patient *patient);
void removeList(struct List *list, struct Patient *patient);

void check_patients_inside(struct Village *village);
void check_patients_waiting(struct Village *village);
void check_patients_realloc(struct Village *village);

void check_patients_assess_par(struct Village *village);
void check_patients_assess_seq(struct Village *village);

float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);

struct Results get_results(struct Village *village);

void sim_village_par(struct Village *village);
void sim_village_seq(struct Village *village);

void sim_village_main_par(struct Village *top);
void sim_village_main_seq(struct Village *top);

int check_village(struct Village *top);

#define NBS_APP_INIT \
   struct Village *top;\
   read_input_data(nbs_arg_file);

#define KERNEL_INIT \
   allocate_village(&top, NULL, NULL, sim_level, 0);

#define KERNEL_CALL sim_village_main_par(top);
 
#define KERNEL_FINI

#define KERNEL_SEQ_INIT \
   allocate_village(&top, NULL, NULL, sim_level, 0);

#define KERNEL_SEQ_CALL sim_village_main_seq(top); 

#define KERNEL_SEQ_FINI

#define KERNEL_CHECK check_village(top);

