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
  
