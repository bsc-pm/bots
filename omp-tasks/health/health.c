/* For copyright information, see olden_v1.0/COPYRIGHT */
/******************************************************************* 
 *  Health.c : Model of the Colombian Health Care System           *
 *******************************************************************/ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "app-desc.h"
#include "nbs.h"

/* global variables */
int  sim_level;
int  sim_cities;
int  sim_population_ratio;
int sim_time;
int sim_assess_time;
int sim_convalescence_time;
long sim_seed;
float sim_get_sick_p;
float sim_convalescence_p;
float sim_realloc_p;
int sim_pid = 0;

int res_population;
int res_hospitals;
int res_personnel;
int res_checkin;
int res_village;
int res_waiting;
int res_assess;
int res_inside;
float res_avg_stay;

/**********************************************************
 * Handles math routines for health.c                     *
 **********************************************************/
float my_rand(long *seed) 
{
   long k;
   float answer;
   long idum = *seed;

   idum ^= MASK;
   k = idum / IQ;
   idum = IA * (idum - k * IQ) - IR * k;
   idum ^= MASK;
   if (idum < 0) idum  += IM;
   answer = (float) AM * idum;
   *seed = (long) (answer * IM);
   return answer; 
}
/********************************************************************
 * Handles lists.                                                   *
 ********************************************************************/
void addList(struct List *list, struct Patient *patient)
{
   struct List *b=NULL;

   while (list != NULL) 
   {
      b = list;
      list = list->forward; 
   }
  
   list = (struct List *)malloc(sizeof(struct List));
   list->patient = patient;
   list->forward = NULL;
   list->back = b;
   b->forward = list; 
} 
/**********************************************************************/
/* TODO: Very naive list implementation */
/**********************************************************************/
void removeList(struct List *list, struct Patient *patient) 
{
   struct List *l1,*l2;
   struct Patient *p;
  
   p = list->patient;
   while(p != patient) 
   {
      list = list->forward; 
      p = list->patient; 
   }
    
   l1 = list->back;
   l2 = list->forward;
   l1->forward = l2;
   if (list->forward != NULL) 
   {
      l1 = list->forward;
      l2 = list->back;
      l1->back = l2; 
   }
}
/**********************************************************************/
void allocate_village( struct Village **capital, struct Village *back,
   struct Village *next, int level, int vid)
{ 
   int i, population, personnel;
   struct Village *current, *inext;
   struct Patient *patient;

   if (level == 0) *capital = NULL;
   else
   {
      personnel = (int) pow(2, level);
      population = personnel * sim_population_ratio;
      /* Allocate Village */
      *capital = (struct Village *) malloc(sizeof(struct Village));
      /* Initialize Village */
      (*capital)->back  = back;
      (*capital)->next  = next;
      (*capital)->level = level;
      (*capital)->id    = vid;
      (*capital)->seed  = vid * (IQ + sim_seed);
      (*capital)->population.back = NULL;
      (*capital)->population.forward = NULL;
      for(i=0;i<population;i++)
      {
         patient = (struct Patient *)malloc(sizeof(struct Patient));
         patient->id = sim_pid++;
         patient->seed = (*capital)->seed;
         // changes seed for capital:
         my_rand(&((*capital)->seed));
         patient->hosps_visited = 0;
         patient->time          = 0;
         patient->time_left     = 0;
         patient->home_village = *capital; 
         addList(&((*capital)->population), patient);
      }
      /* Initialize Hospital */
      (*capital)->hosp.personnel = personnel;
      (*capital)->hosp.free_personnel = personnel;
      (*capital)->hosp.assess.forward = NULL;
      (*capital)->hosp.assess.back = NULL;
      (*capital)->hosp.waiting.forward = NULL;
      (*capital)->hosp.waiting.back = NULL;
      (*capital)->hosp.inside.forward = NULL;
      (*capital)->hosp.inside.back = NULL;
      (*capital)->hosp.realloc.forward = NULL;
      (*capital)->hosp.realloc.back = NULL;
      omp_init_lock(&(*capital)->hosp.realloc_lock);
      // Create Cities (lower level)
      inext = NULL;
      for (i = sim_cities; i>0; i--)
      {
         allocate_village(&current, *capital, inext, level-1, (vid*sim_cities)+i);
         inext = current;
      }
      (*capital)->forward = current;
   }
}
/**********************************************************************/
struct Results get_results(struct Village *village)
{
   struct Village *vlist;
   struct List    *plist;
   struct Patient *p;
   struct Results t_res, p_res;

   t_res.hosps_number     = 0.0;
   t_res.hosps_personnel  = 0.0;
   t_res.total_patients   = 0.0;
   t_res.total_in_village = 0.0;
   t_res.total_waiting    = 0.0;
   t_res.total_assess     = 0.0;
   t_res.total_inside     = 0.0;
   t_res.total_hosps_v    = 0.0;
   t_res.total_time       = 0.0;

   if (village == NULL) return t_res;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   while(vlist)
   {
      p_res = get_results(vlist);
      t_res.hosps_number     += p_res.hosps_number;
      t_res.hosps_personnel  += p_res.hosps_personnel;
      t_res.total_patients   += p_res.total_patients;
      t_res.total_in_village += p_res.total_in_village;
      t_res.total_waiting    += p_res.total_waiting;
      t_res.total_assess     += p_res.total_assess;
      t_res.total_inside     += p_res.total_inside;
      t_res.total_hosps_v    += p_res.total_hosps_v;
      t_res.total_time       += p_res.total_time;
      vlist = vlist->next;
   }
   t_res.hosps_number     += 1.0;
   t_res.hosps_personnel  += village->hosp.personnel;

   // Patients in the village
   plist = village->population.forward;
   while (plist != NULL) 
   {
      p = plist->patient;
      t_res.total_patients   += 1.0;
      t_res.total_in_village += 1.0;
      t_res.total_hosps_v    += (float)(p->hosps_visited);
      t_res.total_time       += (float)(p->time); 
      plist = plist->forward; 
   }
   // Patients in hospital: waiting
   plist = village->hosp.waiting.forward;
   while (plist != NULL) 
   {
      p = plist->patient;
      t_res.total_patients += 1.0;
      t_res.total_waiting  += 1.0;
      t_res.total_hosps_v  += (float)(p->hosps_visited);
      t_res.total_time     += (float)(p->time); 
      plist = plist->forward; 
   }
   // Patients in hospital: assess
   plist = village->hosp.assess.forward;
   while (plist != NULL) 
   {
      p = plist->patient;
      t_res.total_patients += 1.0;
      t_res.total_assess   += 1.0;
      t_res.total_hosps_v  += (float)(p->hosps_visited);
      t_res.total_time     += (float)(p->time); 
      plist = plist->forward; 
   }
   // Patients in hospital: inside
   plist = village->hosp.inside.forward;
   while (plist != NULL) 
   {
      p = plist->patient;
      t_res.total_patients += 1.0;
      t_res.total_inside   += 1.0;
      t_res.total_hosps_v  += (float)(p->hosps_visited);
      t_res.total_time     += (float)(p->time); 
      plist = plist->forward; 
   }  

   return t_res; 
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void check_patients_inside(struct Village *village) 
{
   struct List *list = village->hosp.inside.forward;
   struct Patient *p;
  
   while (list != NULL) 
   {
      p = list->patient;
      p->time_left--;
      
      if (p->time_left == 0) 
      {
         village->hosp.free_personnel++;
         removeList(&(village->hosp.inside), p); 
         addList(&(village->population), p); 
      }    
      list = list->forward; 
   }
}
/**********************************************************************/
void check_patients_assess_par(struct Village *village) 
{
   struct List *list = village->hosp.assess.forward;
   float rand;
   struct Patient *p;

   while (list != NULL) 
   {
      p = list->patient;
      p->time_left--;

      if (p->time_left == 0) 
      { 
         rand = my_rand(&(p->seed));
         /* sim_covalescense_p % */
         if (rand < sim_convalescence_p)
         {
            rand = my_rand(&(p->seed));
            /* !sim_realloc_p % or root hospital */
            if (rand > sim_realloc_p || village->level == sim_level) 
            {
               removeList(&(village->hosp.assess), p);
               addList(&(village->hosp.inside), p);
               p->time_left = sim_convalescence_time;
               p->time += p->time_left;
            }
            else /* move to upper level hospital !!! */
            {
               village->hosp.free_personnel++;
               removeList(&(village->hosp.assess), p);
               omp_set_lock(&(village->hosp.realloc_lock));
               addList(&(village->back->hosp.realloc), p); 
               omp_unset_lock(&(village->hosp.realloc_lock));
            } 
         }
         else /* move to village */
         {
            village->hosp.free_personnel++;
            removeList(&(village->hosp.assess), p);
            addList(&(village->population), p); 
         }
      }
      list = list->forward; 
   } 
}
/**********************************************************************/
void check_patients_assess_seq(struct Village *village) 
{
   struct List *list = village->hosp.assess.forward;
   float rand;
   struct Patient *p;

   while (list != NULL) 
   {
      p = list->patient;
      p->time_left--;

      if (p->time_left == 0) 
      { 
         rand = my_rand(&(p->seed));
         /* sim_covalescense_p % */
         if (rand < sim_convalescence_p)
         {
            rand = my_rand(&(p->seed));
            /* !sim_realloc_p % or root hospital */
            if (rand > sim_realloc_p || village->level == sim_level) 
            {
               removeList(&(village->hosp.assess), p);
               addList(&(village->hosp.inside), p);
               p->time_left = sim_convalescence_time;
               p->time += p->time_left;
            }
            else /* move to upper level hospital !!! */
            {
               village->hosp.free_personnel++;
               removeList(&(village->hosp.assess), p);
               addList(&(village->back->hosp.realloc), p); 
            } 
         }
         else /* move to village */
         {
            village->hosp.free_personnel++;
            removeList(&(village->hosp.assess), p);
            addList(&(village->population), p); 
         }
      }
      list = list->forward; 
   } 
}
/**********************************************************************/
void check_patients_waiting(struct Village *village) 
{
   struct List *list = village->hosp.waiting.forward;
   struct Patient *p;
  
   while (list != NULL) 
   {
      p = list->patient; /* This is a bad load */
      if (village->hosp.free_personnel > 0) 
      {
         village->hosp.free_personnel--;
         p->time_left = sim_assess_time;
         p->time += p->time_left;
         removeList(&(village->hosp.waiting), p);
         addList(&(village->hosp.assess), p); 
      }
      else 
      {
         p->time++; /* so is this */ 
      }
      list = list->forward; 
   } 
}
/**********************************************************************/
void check_patients_realloc(struct Village *village)
{
   struct List *list = village->hosp.realloc.forward;
   struct Patient *p;

   while (list != NULL) 
   {
      p = list->patient;
      removeList(&(village->hosp.realloc), p);
      put_in_hosp(&(village->hosp), p);
      list = list->forward; 
   }
}
/**********************************************************************/
void check_patients_population(struct Village *village) 
{
   struct List *list = village->population.forward;
   struct Patient *p;
   float rand;
  
   while (list != NULL) 
   {
      p = list->patient;
      /* randomize in patient */
      rand = my_rand(&(p->seed));
      if (rand < sim_get_sick_p) 
      {
         removeList(&(village->population), p);
         put_in_hosp(&(village->hosp), p);
      }
      list = list->forward; 
   }

}
/**********************************************************************/
void put_in_hosp(struct Hosp *hosp, struct Patient *patient) 
{  
   (patient->hosps_visited)++;

   if (hosp->free_personnel > 0) 
   {
      hosp->free_personnel--;
      addList(&(hosp->assess), patient); 
      patient->time_left = sim_assess_time;
      patient->time += patient->time_left;
   } 
   else 
   {
      addList(&(hosp->waiting), patient); 
   }
}
/**********************************************************************/
#if defined (IF_CUTOFF)
void sim_village_par(struct Village *village)
{
   struct Village *vlist;

   // lowest level returns nothing
   // only for sim_village first call with village = NULL
   // recursive call cannot occurs
   if (village == NULL) return;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   while(vlist)
   {
#pragma omp task untied if((sim_level - village->level) < nbs_cutoff_value)
      sim_village_par(vlist);
      vlist = vlist->next;
   }

#pragma omp taskwait

   /* Uses lists v->hosp->inside, and v->return */
   check_patients_inside(village);

   /* Uses lists v->hosp->assess, v->hosp->inside, v->population and (v->back->hosp->up) !!! */
   check_patients_assess_par(village);

   /* Uses lists v->hosp->waiting, and v->hosp->assess */
   check_patients_waiting(village);

   /* Uses lists v->hosp->realloc, v->hosp->asses and v->hosp->waiting */
   check_patients_realloc(village);

   /* Uses list v->population, v->hosp->asses and v->h->waiting */
   check_patients_population(village);
}
#elif defined (MANUAL_CUTOFF)
void sim_village_par(struct Village *village)
{
   struct Village *vlist;

   // lowest level returns nothing
   // only for sim_village first call with village = NULL
   // recursive call cannot occurs
   if (village == NULL) return;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   if ((sim_level-village->level) < nbs_cutoff_value)
   {
      while(vlist)
      {
#pragma omp task untied
         sim_village_par(vlist);
         vlist = vlist->next;
      }
#pragma omp taskwait
   }
   else
   {
      while(vlist)
      {
         sim_village_par(vlist);
         vlist = vlist->next;
      }
   }

   /* Uses lists v->hosp->inside, and v->return */
   check_patients_inside(village);

   /* Uses lists v->hosp->assess, v->hosp->inside, v->population and (v->back->hosp->up) !!! */
   check_patients_assess_par(village);

   /* Uses lists v->hosp->waiting, and v->hosp->assess */
   check_patients_waiting(village);

   /* Uses lists v->hosp->realloc, v->hosp->asses and v->hosp->waiting */
   check_patients_realloc(village);

   /* Uses list v->population, v->hosp->asses and v->h->waiting */
   check_patients_population(village);
}
#else
void sim_village_par(struct Village *village)
{
   struct Village *vlist;

   // lowest level returns nothing
   // only for sim_village first call with village = NULL
   // recursive call cannot occurs
   if (village == NULL) return;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   while(vlist)
   {
#pragma omp task untied
      sim_village_par(vlist);
      vlist = vlist->next;
   }

#pragma omp taskwait

   /* Uses lists v->hosp->inside, and v->return */
   check_patients_inside(village);

   /* Uses lists v->hosp->assess, v->hosp->inside, v->population and (v->back->hosp->up) !!! */
   check_patients_assess_par(village);

   /* Uses lists v->hosp->waiting, and v->hosp->assess */
   check_patients_waiting(village);

   /* Uses lists v->hosp->realloc, v->hosp->asses and v->hosp->waiting */
   check_patients_realloc(village);

   /* Uses list v->population, v->hosp->asses and v->h->waiting */
   check_patients_population(village);
}
#endif
/**********************************************************************/
void sim_village_seq(struct Village *village)
{
   struct Village *vlist;

   // lowest level returns nothing
   // only for sim_village first call with village = NULL
   // recursive call cannot occurs
   if (village == NULL) return;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   while(vlist)
   {
      sim_village_seq(vlist);
      vlist = vlist->next;
   }

   /* Uses lists v->hosp->inside, and v->return */
   check_patients_inside(village);

   /* Uses lists v->hosp->assess, v->hosp->inside, v->population and (v->back->hosp->up) !!! */
   check_patients_assess_seq(village);

   /* Uses lists v->hosp->waiting, and v->hosp->assess */
   check_patients_waiting(village);

   /* Uses lists v->hosp->realloc, v->hosp->asses and v->hosp->waiting */
   check_patients_realloc(village);

   /* Uses list v->population, v->hosp->asses and v->h->waiting */
   check_patients_population(village);
}
/**********************************************************************/
void my_print(struct Village *village)
{
   struct Village *vlist;
   struct List    *plist;
   struct Patient *p;

   if (village == NULL) return;

   /* Traverse village hierarchy (lower level first)*/
   vlist = village->forward;
   while(vlist)
   {
      my_print(vlist);
      vlist = vlist->next;
   }

   plist = village->population.forward;

   while (plist != NULL) 
   {
      p = plist->patient;
      fprintf(stderr,"[pid:%d]",p->id);
      plist = plist->forward; 
   }
   fprintf(stderr,"[vid:%d]\n",village->id);

}
/**********************************************************************/
void read_input_data(char *filename)
{
   FILE *fin;

   if ((fin = fopen(filename, "r")) == NULL) {
      fprintf(stdout, "Could not open sequence file (%s)\n", filename);
      exit (-1);
   }
   fscanf(fin,"%d %d %d %d %d %d %ld %f %f %f %d %d %d %d %d %d %d %d %f", 
             &sim_level,
             &sim_cities,
             &sim_population_ratio,
             &sim_time, 
             &sim_assess_time,
             &sim_convalescence_time,
             &sim_seed, 
             &sim_get_sick_p,
             &sim_convalescence_p,
             &sim_realloc_p,
             &res_population,
             &res_hospitals,
             &res_personnel,
             &res_checkin,
             &res_village,
             &res_waiting,
             &res_assess,
             &res_inside,
             &res_avg_stay
   );
   fclose(fin);

   if (nbs_verbose_mode)
   {
      // Printing input data
      fprintf(stdout,"\n");
      fprintf(stdout,"Number of levels    = %d\n", (int) sim_level);
      fprintf(stdout,"Cities per level    = %d\n", (int) sim_cities);
      fprintf(stdout,"Population ratio    = %d\n", (int) sim_population_ratio);
      fprintf(stdout,"Simulation time     = %d\n", (int) sim_time);
      fprintf(stdout,"Assess time         = %d\n", (int) sim_assess_time);
      fprintf(stdout,"Convalescence time  = %d\n", (int) sim_convalescence_time);
      fprintf(stdout,"Initial seed        = %d\n", (int) sim_seed);
      fprintf(stdout,"Get sick prob.      = %f\n", (float) sim_get_sick_p);
      fprintf(stdout,"Convalescence prob. = %f\n", (float) sim_convalescence_p);
      fprintf(stdout,"Realloc prob.       = %f\n", (float) sim_realloc_p);
   }
}
int check_village(struct Village *top)
{
   struct Results result = get_results(top);
   int answer = NBS_RESULT_SUCCESSFUL;

   if (res_population != result.total_patients) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_hospitals != result.hosps_number) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_personnel != result.hosps_personnel) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_checkin != result.total_hosps_v) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_village != result.total_in_village) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_waiting != result.total_waiting) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_assess != result.total_assess) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_inside != result.total_inside) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_avg_stay > (float) (result.total_time/result.total_patients)+0.005) answer = NBS_RESULT_UNSUCCESSFUL;
   if (res_avg_stay < (float) (result.total_time/result.total_patients)-0.005) answer = NBS_RESULT_UNSUCCESSFUL;
   if (nbs_verbose_mode)
   {
      fprintf(stdout,"\n");
      fprintf(stdout,"Total population    = %6d / %6d people\n", (int)   res_population, (int) result.total_patients);
      fprintf(stdout,"Hospitals           = %6d / %6d people\n", (int)   res_hospitals, (int) result.hosps_number);
      fprintf(stdout,"Personnel           = %6d / %6d people\n", (int)   res_personnel, (int) result.hosps_personnel);
      fprintf(stdout,"Check-in's          = %6d / %6d people\n", (int)   res_checkin, (int) result.total_hosps_v);
      fprintf(stdout,"In Villages         = %6d / %6d people\n", (int)   res_village, (int) result.total_in_village);
      fprintf(stdout,"In Waiting List     = %6d / %6d people\n", (int)   res_waiting, (int) result.total_waiting);
      fprintf(stdout,"In Assess           = %6d / %6d people\n", (int)   res_assess, (int) result.total_assess);
      fprintf(stdout,"Inside Hospital     = %6d / %6d people\n", (int)   res_inside, (int) result.total_inside);
      fprintf(stdout,"Average Stay        = %6f / %6f u/time\n", (float) res_avg_stay,(float) result.total_time/result.total_patients);
   }
   return answer;
}
/**********************************************************************/
void sim_village_main_par(struct Village *top)
{
   long i;
#pragma omp parallel
#pragma omp single
#pragma omp task untied
   for (i = 0; i < sim_time; i++) sim_village_par(top);   
}
/**********************************************************************/
void sim_village_main_seq(struct Village *top)
{
   long i;
   for (i = 0; i < sim_time; i++) sim_village_seq(top);   
}
