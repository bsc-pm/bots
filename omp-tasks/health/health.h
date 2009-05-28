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
#ifndef _HEALTH_H
#define _HEALTH_H
/* random defines */
#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

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

float my_rand(long *seed);

struct Patient *generate_patient(struct Village *village);
void put_in_hosp(struct Hosp *hosp, struct Patient *patient);

void addList(struct Patient **list, struct Patient *patient);
void removeList(struct Patient **list, struct Patient *patient);

void check_patients_inside(struct Village *village);
void check_patients_waiting(struct Village *village);
void check_patients_realloc(struct Village *village);

void check_patients_assess_par(struct Village *village);

float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);

struct Results get_results(struct Village *village);

void sim_village_par(struct Village *village);
#endif
