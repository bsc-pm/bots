#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/resource.h>

#include "nbs_common.h"
#include "nbs_main.h"
#include "nbs.h"

void
nbs_error(int error, char *message)
{
   char error_message[128];
   if (message == NULL)
   {
      switch(error)
      {
         case NBS_ERROR:
            fprintf(stderr, "Error (%d): %s\n",error,"Unspecified error.");
            break;
         case NBS_ERROR_NOT_ENOUGH_MEMORY:
            fprintf(stderr, "Error (%d): %s\n",error,"Not enough memory.");
            break;
         case NBS_ERROR_UNRECOGNIZED_PARAMETER:
            fprintf(stderr, "Error (%d): %s\n",error,"Unrecognized parameter.");
            nbs_print_usage();
            break;
      }
   }
   else
   {
      fprintf(stderr, "Error (%d): %s\n",error,message);
   }

   exit(100+error);
}

void
nbs_warning(int warning, char *message)
{

}

void
nbs_get_date(char *str)
{
   time_t now;
   time(&now);
   strftime(str, 32, "%Y/%m/%d;%H:%M", gmtime(&now));
}

int
nbs_get_max_cpus(void)
{
#if 0
   return sysconf(_SC_NPROCESSORS_CONF);
#endif
   return 0;
}

void
nbs_get_architecture(char *str)
{
#if 0
   struct utsname *architecture;
   uname(architecture);
   sprintf(str, "%s-%s;%d"
      ,architecture->sysname
      ,architecture->machine
      ,nbs_get_max_cpus());
#endif
   sprintf(str,";");
}

void
nbs_get_load_average(char *str)
{
   double loadavg[3];
   getloadavg (loadavg, 3);
   sprintf(str, "%.2f;%.2f;%.2f",loadavg[0],loadavg[1],loadavg[2]);
}

long
nbs_usecs (void)
{
   struct timeval t;
   gettimeofday(&t,NULL);
   return t.tv_sec*1000000+t.tv_usec;
}

void
nbs_print_header_row()
{
}

void nbs_print_results()
{
   char str_name[128];
   char str_parameters[128];
   char str_model[128];
   char str_resources[128];
   char str_result[15];
   char str_time_program[15];
   char str_time_sequential[15];
   char str_speed_up[15];
   char str_number_of_tasks[15];
   char str_number_of_tasks_per_second[15];
   char str_exec_date[15];
   char str_exec_message[128];
   char str_architecture[128];
   char str_load_avg[128];
   char str_comp_date[128];
   char str_comp_message[128];
   char str_cc[128];
   char str_cflags[128];
   char str_ld[128];
   char str_ldflags[128];
   char str_cutoff[128];

   /* compute output strings */
   sprintf(str_name, "%s", nbs_name);
   sprintf(str_parameters, "%s", nbs_parameters);
   sprintf(str_model, "%s", nbs_model);
   sprintf(str_cutoff, "%s", nbs_cutoff);
   sprintf(str_resources, "%s", nbs_resources);
   switch(nbs_result)
   {
      case NBS_RESULT_NA: 
         sprintf(str_result, "n/a");
         break;
      case NBS_RESULT_SUCCESSFUL: 
         sprintf(str_result, "successful");
         break;
      case NBS_RESULT_UNSUCCESSFUL: 
         sprintf(str_result, "UNSUCCESSFUL");
         break;
      default: 
         sprintf(str_result, "error");
         break;
   }
   sprintf(str_time_program, "%f", nbs_time_program);
   if (nbs_sequential_flag) sprintf(str_time_sequential, "%f", nbs_time_sequential);
   else sprintf(str_time_sequential, "n/a");
   if (nbs_sequential_flag)
   sprintf(str_speed_up, "%3.2f", nbs_time_sequential/nbs_time_program);
   else sprintf(str_speed_up, "n/a");

   sprintf(str_number_of_tasks, "%3.2f", (float) nbs_number_of_tasks);
   sprintf(str_number_of_tasks_per_second, "%3.2f", (float) nbs_number_of_tasks/nbs_time_program);

   sprintf(str_exec_date, "%s", nbs_exec_date);
   sprintf(str_exec_message, "%s", nbs_exec_message);
   nbs_get_architecture(str_architecture);
   nbs_get_load_average(str_load_avg);
   sprintf(str_comp_date, "%s", nbs_comp_date);
   sprintf(str_comp_message, "%s", nbs_comp_message);
   sprintf(str_cc, "%s", nbs_cc);
   sprintf(str_cflags, "%s", nbs_cflags);
   sprintf(str_ld, "%s", nbs_ld);
   sprintf(str_ldflags, "%s", nbs_ldflags);

   if(nbs_print_header)
   {
      switch(nbs_output_format)
      {
         case 0:
            break;
         case 1:
fprintf(stdout,
"Benchmark;Parameters;Model;Cutoff;Resources;Result;\
Time;Sequential;Speed-up;\
Exec Date;Exec Time;Exec Message;\
Architecture;Processors;Load Avg-1;Load Avg-5;Load Avg-15;\
Comp Date;Comp Time;Comp Message;CC;CFLAGS;LD;LDFLAGS\n");
            break;
         default:
            break;
      }
   }

   /* print results */
   switch(nbs_output_format)
   {
      case 0:
	 fprintf(stdout, "\n\n");
         fprintf(stdout, "Program             = %s\n", str_name); /*fix*/
         fprintf(stdout, "Parameters          = %s\n", str_parameters); /*fix*/
         fprintf(stdout, "Model               = %s\n", str_model); 
         fprintf(stdout, "Embedded cut-off    = %s\n", str_cutoff); 
         fprintf(stdout, "# of Threads        = %s\n", str_resources);
         fprintf(stdout, "Verification        = %s\n", str_result);

         fprintf(stdout, "Time Program        = %s seconds\n", str_time_program);
         fprintf(stdout, "Time Sequential     = %s seconds\n", str_time_sequential);
         fprintf(stdout, "Speed-up            = %s\n", str_speed_up);

         fprintf(stdout, "Tasks               = %s\n", str_number_of_tasks);
         fprintf(stdout, "Tasks/Sec           = %s\n", str_number_of_tasks_per_second);

         fprintf(stdout, "Execution Date      = %s\n", str_exec_date);
         fprintf(stdout, "Execution Message   = %s\n", str_exec_message);

         fprintf(stdout, "Architecture        = %s\n", str_architecture);
         fprintf(stdout, "Load Avg [1:5:15]   = %s\n", str_load_avg);

         fprintf(stdout, "Compilation Date    = %s\n", str_comp_date);
         fprintf(stdout, "Compilation Message = %s\n", str_comp_message);

         fprintf(stdout, "Compiler            = %s\n", str_cc);
         fprintf(stdout, "Compiler Flags      = %s\n", str_cflags);
         fprintf(stdout, "Linker              = %s\n", str_ld);
         fprintf(stdout, "Linker Flags        = %s\n", str_ldflags);
	 fflush(stdout);
         break;
      case 1:
         fprintf(stdout,"%s;%s;%s;%s;%s;%s;", 
              str_name, 
              str_parameters, 
              str_model, 
              str_cutoff, 
              str_resources, 
              str_result
         );
         fprintf(stdout,"%s;%s;%s;", 
              str_time_program, 
              str_time_sequential, 
              str_speed_up 
         );
         fprintf(stdout,"%s;%s;", 
              str_exec_date,
              str_exec_message
         );
         fprintf(stdout,"%s;%s;", 
              str_architecture,
              str_load_avg
         );
         fprintf(stdout,"%s;%s;", 
              str_comp_date,
              str_comp_message
         );
         fprintf(stdout,"%s;%s;%s;%s;",
              str_cc,
              str_cflags,
              str_ld,
              str_ldflags
         );
         fprintf(stdout,"\n");
         break;
      default:
         nbs_error(NBS_ERROR,"No valid output format\n");
         break;
   }
}

