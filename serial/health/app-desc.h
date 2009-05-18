/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

#include "serial-app.h"

#define BOTS_APP_NAME "Health"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file

//#define BOTS_APP_SELF_TIMING

#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DEF_ARG_FILE "Input filename"
#define BOTS_APP_DESC_ARG_FILE "Health input file (mandatory)"

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
   struct Patient *back;
   struct Patient *forward;
};
struct Hosp {
   int personnel;
   int free_personnel;
   struct Patient *waiting;
   struct Patient *assess;
   struct Patient *inside;
   struct Patient *realloc;
   omp_lock_t  realloc_lock;
};
struct Village {
   int id;
   struct Village *back;
   struct Village *next;
   struct Village *forward;
   struct Patient *population;
   struct Hosp hosp;
   int level;
   long  seed;
};

void read_input_data(char *filename);
float my_rand(long *seed);

void allocate_village( struct Village **capital, struct Village *back, struct Village *next, int level, int vid);

struct Patient *generate_patient(struct Village *village);
void put_in_hosp(struct Hosp *hosp, struct Patient *patient);

void addList(struct Patient **list, struct Patient *patient);
void removeList(struct Patient **list, struct Patient *patient);

void check_patients_inside(struct Village *village);
void check_patients_waiting(struct Village *village);
void check_patients_realloc(struct Village *village);

void check_patients_assess(struct Village *village);

float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);

struct Results get_results(struct Village *village);

void sim_village(struct Village *village);

void sim_village_main(struct Village *top);

int check_village(struct Village *top);

#define BOTS_APP_INIT \
   struct Village *top;\
   read_input_data(bots_arg_file);

#define KERNEL_INIT \
   allocate_village(&top, NULL, NULL, sim_level, 0);

#define KERNEL_CALL sim_village_main(top);
 
#define KERNEL_FINI

//#define KERNEL_SEQ_INIT 
//#define KERNEL_SEQ_CALL
//#define KERNEL_SEQ_FINI

#define KERNEL_CHECK check_village(top);

