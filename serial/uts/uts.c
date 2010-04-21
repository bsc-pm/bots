/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/**********************************************************************************************/
/*
 * Copyright (c) 2007 The Unbalanced Tree Search (UTS) Project Team:
 * -----------------------------------------------------------------
 *  
 *  This file is part of the unbalanced tree search benchmark.  This
 *  project is licensed under the MIT Open Source license.  See the LICENSE
 *  file for copyright and licensing information.
 *
 *  UTS is a collaborative project between researchers at the University of
 *  Maryland, the University of North Carolina at Chapel Hill, and the Ohio
 *  State University.
 *
 * University of Maryland:
 *   Chau-Wen Tseng(1)  <tseng at cs.umd.edu>
 *
 * University of North Carolina, Chapel Hill:
 *   Jun Huan         <huan,
 *   Jinze Liu         liu,
 *   Stephen Olivier   olivier,
 *   Jan Prins*        prins at cs.umd.edu>
 * 
 * The Ohio State University:
 *   James Dinan      <dinan,
 *   Gerald Sabin      sabin,
 *   P. Sadayappan*    saday at cse.ohio-state.edu>
 *
 * Supercomputing Research Center
 *   D. Pryor
 *
 * (1) - indicates project PI
 *
 * UTS Recursive Depth-First Search (DFS) version developed by James Dinan
 *
 * Adapted for OpenMP 3.0 Task-based version by Stephen Olivier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#include "app-desc.h"
#include "bots.h"
#include "uts.h"

/***********************************************************
 *  Global state                                           *
 ***********************************************************/
counter_t nLeaves = 0;
int maxTreeDepth = 0;
/***********************************************************
 *  tree generation and search parameters                  *
 *                                                         *
 *  Tree generation strategy is controlled via various     *
 *  parameters set from the command line.  The parameters  *
 *  and their default values are given below.              *
 ***********************************************************/
char * uts_trees_str[]     = { "Binomial" };
/***********************************************************
 * Tree type
 *   Trees are generated using a Galton-Watson process, in 
 *   which the branching factor of each node is a random 
 *   variable.
 *   
 *   The random variable follow a binomial distribution.
 ***********************************************************/
tree_t type  = BIN; // Default tree type
double b_0   = 4.0; // default branching factor at the root
int   rootId = 0;   // default seed for RNG state at root
/***********************************************************
 *  Tree type BIN (BINOMIAL)
 *  The branching factor at the root is specified by b_0.
 *  The branching factor below the root follows an 
 *     identical binomial distribution at all nodes.
 *  A node has m children with prob q, or no children with 
 *     prob (1-q).  The expected branching factor is q * m.
 *
 *  Default parameter values 
 ***********************************************************/
int    nonLeafBF   = 4;            // m
double nonLeafProb = 15.0 / 64.0;  // q
/***********************************************************
 * compute granularity - number of rng evaluations per
 * tree node
 ***********************************************************/
int computeGranularity = 1;
/***********************************************************
 * expected results for execution
 ***********************************************************/
int        exp_tree_size = 0;
int        exp_tree_depth = 0;
counter_t  exp_num_leaves = 0;
/***********************************************************
 *  FUNCTIONS                                              *
 ***********************************************************/

// Interpret 32 bit positive integer as value on [0,1)
double rng_toProb(int n)
{
  if (n < 0) {
    printf("*** toProb: rand n = %d out of range\n",n);
  }
  return ((n<0)? 0.0 : ((double) n)/2147483648.0);
}

void uts_initRoot(Node * root, int type)
{
   root->height = 0;
   root->numChildren = -1;      // means not yet determined
   rng_init(root->state.state, rootId);

   printf("Root node of type %d at %p\n",type, root);
}


int uts_numChildren_bin(Node * parent)
{
  // distribution is identical everywhere below root
  int    v = rng_rand(parent->state.state);	
  double d = rng_toProb(v);

  return (d < nonLeafProb) ? nonLeafBF : 0;
}

int uts_numChildren(Node *parent)
{
  int numChildren = 0;

  /* Determine the number of children */
  if (parent->height == 0)
     numChildren = (int) floor(b_0);
  else 
     numChildren = uts_numChildren_bin(parent);
  
  // limit number of children
  // only a BIN root can have more than MAXNUMCHILDREN
  if (parent->height == 0) {
    int rootBF = (int) ceil(b_0);
    if (numChildren > rootBF) {
      message("*** Number of children of root truncated from %d to %d\n", numChildren, rootBF);
      numChildren = rootBF;
    }
  }
  else {
    if (numChildren > MAXNUMCHILDREN) {
      message("*** Number of children truncated from %d to %d\n", numChildren, MAXNUMCHILDREN);
      numChildren = MAXNUMCHILDREN;
    }
  }

  return numChildren;
}

/***********************************************************
 * Recursive depth-first implementation                    *
 ***********************************************************/

int getNumRootChildren(Node *root)
{
  int numChildren;

  numChildren = uts_numChildren(root);
  root->numChildren = numChildren;

  return numChildren;
}

counter_t serial_uts ( Node *root )
{
   counter_t num_nodes;
   message("Computing Unbalance Tree Search algorithm ");
   num_nodes = serTreeSearch( 0, root, getNumRootChildren(root) );
   message(" completed!\n");
   return num_nodes;
}

counter_t serTreeSearch(int depth, Node *parent, int numChildren) 
{
  Node n[numChildren], *nodePtr;
  int i, j;
  counter_t subtreesize = 1, partialCount[numChildren];

  // Recurse on the children
  for (i = 0; i < numChildren; i++) {
     nodePtr = &n[i];

     nodePtr->height = parent->height + 1;

     // The following line is the work (one or more SHA-1 ops)
     for (j = 0; j < computeGranularity; j++) {
        rng_spawn(parent->state.state, nodePtr->state.state, i);
     }

     nodePtr->numChildren = uts_numChildren(nodePtr);

        partialCount[i] = serTreeSearch(depth+1, nodePtr, nodePtr->numChildren);
  }

  for (i = 0; i < numChildren; i++) {
     subtreesize += partialCount[i];
  }
  
  return subtreesize;
}

void uts_read_file ( char *filename )
{
   FILE *fin;

   if ((fin = fopen(filename, "r")) == NULL) {
      message( "Could not open input file (%s)\n", filename);
      exit (-1);
   }
   fscanf(fin,"%lf %lf %d %d %d %llu %d %llu",
             &b_0,
             &nonLeafProb,
             &nonLeafBF,
             &rootId,
             &computeGranularity,
             &exp_tree_size,
             &exp_tree_depth,
             &exp_num_leaves
   );
   fclose(fin);

   computeGranularity = max(1,computeGranularity);

      // Printing input data
   message("\n");
   message("Root branching factor                = %f\n", b_0);
   message("Root seed (0 <= 2^31)                = %d\n", rootId);
   message("Probability of non-leaf node         = %f\n", nonLeafProb);
   message("Number of children for non-leaf node = %d\n", nonLeafBF);
   message("E(n)                                 = %f\n", (double) ( nonLeafProb * nonLeafBF ) );
   message("E(s)                                 = %f\n", (double) ( 1.0 / (1.0 - nonLeafProb * nonLeafBF) ) );
   message("Compute granularity                  = %d\n", computeGranularity);
   message("Tree type                            = %d (%s)\n", type, uts_trees_str[type]);
   message("Random number generator              = "); rng_showtype();
}

void uts_show_stats( void )
{
   int nPes = atoi(bots_resources);
   int chunkSize = 0;

   message("\n");
   message("Tree size                            = %llu\n", bots_number_of_tasks );
   message("Maximum tree depth                   = %d\n", maxTreeDepth );
   message("Chunk size                           = %d\n", chunkSize );
   message("Number of leaves                     = %llu (%.2f%%)\n", nLeaves, nLeaves/(float)bots_number_of_tasks*100.0 ); 
   message("Wallclock time                       = %.3f sec\n", bots_time_program );
   message("Overall performance                  = %.0f nodes/sec\n", (bots_number_of_tasks / bots_time_program) );
}

int uts_check_result ( void )
{
   int answer = BOTS_RESULT_SUCCESSFUL;

   if ( bots_number_of_tasks != exp_tree_size ) {
      answer = BOTS_RESULT_UNSUCCESSFUL;
      message("Tree size value is non valid.\n");
   }

// These variables are not computed in current implementation but are included in test file
// Tree is allocated in stack frame during execution and cannot be chased again
#if 0
   if ( maxTreeDepth != exp_tree_depth ) {
      answer = BOTS_RESULT_UNSUCCESSFUL;
      message("Tree depth value is non valid.\n");
   }

   if ( nLeaves != exp_num_leaves ) {
      answer = BOTS_RESULT_UNSUCCESSFUL;
      message("Number of leaves is non valid.\n");
   }
#endif

   return answer;
}
