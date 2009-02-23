#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app-desc.h"

#define ROWS 64
#define COLS 64
#define DMAX 64
#define verbose 0
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

int max_level = 50;

typedef int  coor[2];
typedef char ibrd[ROWS][COLS];

FILE * inputFile;

struct cell {
  int   n;
  coor *alt;
  int   top;
  int   bot;
  int   lhs;
  int   rhs;
  int   left;
  int   above;
  int   next;
};

struct cell * gcells;

int  MIN_AREA;
ibrd BEST_BOARD;
coor MIN_FOOTPRINT;

int N;

/* compute all possible locations for nw corner for cell */
static int starts(int id, int shape, coor *NWS, struct cell *cells) {
  int i, n, top, bot, lhs, rhs;
  int rows, cols, left, above;

/* size of cell */
  rows  = cells[id].alt[shape][0];
  cols  = cells[id].alt[shape][1];

/* the cells to the left and above */
  left  = cells[id].left;
  above = cells[id].above;

/* if there is a vertical and horizontal dependence */
  if ((left >= 0) && (above >= 0)) {

     top = cells[above].bot + 1;
     lhs = cells[left].rhs + 1;
     bot = top + rows;
     rhs = lhs + cols;

/* if footprint of cell touches the cells to the left and above */
     if ((top <= cells[left].bot) && (bot >= cells[left].top) &&
         (lhs <= cells[above].rhs) && (rhs >= cells[above].lhs))
          { n = 1; NWS[0][0] = top; NWS[0][1] = lhs;  }
     else { n = 0; }

/* if there is only a horizontal dependence */
   } else if (left >= 0) {

/* highest initial row is top of cell to the left - rows */ 
     top = max(cells[left].top - rows + 1, 0);
/* lowest initial row is bottom of cell to the left */
     bot = min(cells[left].bot, ROWS);
     n   = bot - top + 1;

     for (i = 0; i < n; i++) {
         NWS[i][0] = i + top;
         NWS[i][1] = cells[left].rhs + 1;
     }

  } else {

/* leftmost initial col is lhs of cell above - cols */
     lhs = max(cells[above].lhs - cols + 1, 0);
/* rightmost initial col is rhs of cell above */
     rhs = min(cells[above].rhs, COLS);
     n   = rhs - lhs + 1;

     for (i = 0; i < n; i++) {
         NWS[i][0] = cells[above].bot + 1;
         NWS[i][1] = i + lhs;
  }  }

  return (n);
}



/* lay the cell down on the board in the rectangular space defined
   by the cells top, bottom, left, and right edges. If the cell can
   not be layed down, return 0; else 1.
*/
static int lay_down(int id, ibrd board, struct cell *cells) {
  int  i, j, top, bot, lhs, rhs;

  top = cells[id].top;
  bot = cells[id].bot;
  lhs = cells[id].lhs;
  rhs = cells[id].rhs;

  for (i = top; i <= bot; i++) {
  for (j = lhs; j <= rhs; j++) {
      if (board[i][j] == 0) board[i][j] = (char)id;
      else                  return(0);
  } }

  return (1);
}


static void read_inputs() {
  int i, j, n;

  fscanf(inputFile, "%d", &n);
  N = n;
  
  gcells = (struct cell *) malloc((n + 1) * sizeof(struct cell));

  gcells[0].n     =  0;
  gcells[0].alt   =  0;
  gcells[0].top   =  0;
  gcells[0].bot   =  0;
  gcells[0].lhs   = -1;
  gcells[0].rhs   = -1;
  gcells[0].left  =  0;
  gcells[0].above =  0;
  gcells[0].next  =  0;

  for (i = 1; i < n + 1; i++) {

      fscanf(inputFile, "%d", & (gcells[i].n));
      gcells[i].alt = (coor *) malloc(gcells[i].n * sizeof(coor));

      for (j = 0; j < gcells[i].n; j++) {
          fscanf(inputFile, "%d", & (gcells[i].alt[j][0]));
          fscanf(inputFile, "%d", & (gcells[i].alt[j][1]));
      }

      fscanf(inputFile, "%d", & (gcells[i].left));
      fscanf(inputFile, "%d", & (gcells[i].above));
      fscanf(inputFile, "%d", & (gcells[i].next));
      }
}


static void write_outputs() {
  int i, j;

  printf("Minimum area = %d\n\n", MIN_AREA);

  for (i = 0; i < MIN_FOOTPRINT[0]; i++) {
  for (j = 0; j < MIN_FOOTPRINT[1]; j++) {
      if (BEST_BOARD[i][j] == 0) printf(" ");
      else                       printf("%c", 'A' + BEST_BOARD[i][j] - 1);
  } 
  printf("\n");
} }


static void add_cell_ser (int id, coor FOOTPRINT, ibrd BOARD, struct cell *CELLS) {
  int  i, j, nn, area;

  ibrd board;
  coor footprint, NWS[DMAX];

/* for each possible shape */
  for (i = 0; i < CELLS[id].n; i++) {
/* compute all possible locations for nw corner */
      nn = starts(id, i, NWS, CELLS);
/* for all possible locations */
      for (j = 0; j < nn; j++) {
	  struct cell cells[N+1];

	  memcpy(cells,CELLS,sizeof(struct cell)*(N+1));
/* extent of shape */
          cells[id].top = NWS[j][0];
          cells[id].bot = cells[id].top + cells[id].alt[i][0] - 1;
          cells[id].lhs = NWS[j][1];
          cells[id].rhs = cells[id].lhs + cells[id].alt[i][1] - 1;

          memcpy(board, BOARD, sizeof(ibrd));

/* if the cell cannot be layed down, prune search */
          if (! lay_down(id, board, cells)) {
             if (verbose) printf("Chip %d, shape %d does not fit\n", id, i);
             goto _end;
          }

/* calculate new footprint of board and area of footprint */
          footprint[0] = max(FOOTPRINT[0], cells[id].bot);
          footprint[1] = max(FOOTPRINT[1], cells[id].rhs);
          area         = footprint[0] * footprint[1];

/* if last cell */
          if (cells[id].next == 0) {

/* if area is minimum, update global values */
		  if (area < MIN_AREA) {
#pragma omp critical
			  if (area < MIN_AREA) {
				  MIN_AREA         = area;
				  MIN_FOOTPRINT[0] = footprint[0];
				  MIN_FOOTPRINT[1] = footprint[1];
				  memcpy(BEST_BOARD, board, sizeof(ibrd));
				  if (verbose) printf("N  %d\n", MIN_AREA);
			  }
		  }

/* if area is less than best area */
          } else if (area < MIN_AREA) {
             add_cell_ser(cells[id].next, footprint, board,cells);

/* if area is greater than or equal to best area, prune search */
          } else {

             if (verbose) printf("T  %d, %d\n", area, MIN_AREA);
 
	  }
_end:;  
}
}
  }


static void add_cell(int id, coor FOOTPRINT, ibrd BOARD, struct cell *CELLS,int level) {
  int  i, j, nn, area;

  ibrd board;
  coor footprint, NWS[DMAX];

/* for each possible shape */
  for (i = 0; i < CELLS[id].n; i++) {
/* compute all possible locations for nw corner */
      nn = starts(id, i, NWS, CELLS);
/* for all possible locations */
      for (j = 0; j < nn; j++) {
#pragma omp task default(none) untied private(board, footprint,area) \
firstprivate(NWS,i,j,id,nn,level) shared(max_level) \
shared(FOOTPRINT,BOARD,CELLS,MIN_AREA,MIN_FOOTPRINT,N,BEST_BOARD) 
{
	  struct cell cells[N+1];

	  memcpy(cells,CELLS,sizeof(struct cell)*(N+1));
/* extent of shape */
          cells[id].top = NWS[j][0];
          cells[id].bot = cells[id].top + cells[id].alt[i][0] - 1;
          cells[id].lhs = NWS[j][1];
          cells[id].rhs = cells[id].lhs + cells[id].alt[i][1] - 1;

          memcpy(board, BOARD, sizeof(ibrd));

/* if the cell cannot be layed down, prune search */
          if (! lay_down(id, board, cells)) {
             if (verbose) printf("Chip %d, shape %d does not fit\n", id, i);
             goto _end;
          }

/* calculate new footprint of board and area of footprint */
          footprint[0] = max(FOOTPRINT[0], cells[id].bot);
          footprint[1] = max(FOOTPRINT[1], cells[id].rhs);
          area         = footprint[0] * footprint[1];

/* if last cell */
          if (cells[id].next == 0) {

/* if area is minimum, update global values */
		  if (area < MIN_AREA) {
#pragma omp critical
			  if (area < MIN_AREA) {
				  MIN_AREA         = area;
				  MIN_FOOTPRINT[0] = footprint[0];
				  MIN_FOOTPRINT[1] = footprint[1];
				  memcpy(BEST_BOARD, board, sizeof(ibrd));
				  if (verbose) printf("N  %d\n", MIN_AREA);
			  }
		  }

/* if area is less than best area */
          } else if (area < MIN_AREA) {
//#pragma omp task untied firsprivate(board,footprint,cells)
	     if(level+1 < max_level )
                add_cell(cells[id].next, footprint, board,cells,level+1);
	     else
		add_cell_ser(cells[id].next, footprint, board,cells);

/* if area is greater than or equal to best area, prune search */
          } else {

             if (verbose) printf("T  %d, %d\n", area, MIN_AREA);
 
	  }
_end:;  
}
      }
}
#pragma omp taskwait

}

ibrd board;

void floorplan_init (char *filename)
{
    int i,j;

    inputFile = fopen(filename, "r");
    
    if(NULL == inputFile) {
        printf("couldn't open %s for reading\n", filename);
        exit(1);
    }
    
    /* read input file and initialize global minimum area */
    read_inputs();
    MIN_AREA = ROWS * COLS;
    
    /* initialize board is empty */
    for (i = 0; i < ROWS; i++)
    for (j = 0; j < COLS; j++) board[i][j] = 0;
    
}

void compute_floorplan (void)
{
    coor footprint;
    /* footprint of initial board is zero */
    footprint[0] = 0;
    footprint[1] = 0;

#pragma omp parallel
#pragma omp single
    add_cell(1, footprint, board, gcells,0);
}

void floorplan_end ()
{
    /* write results */
    write_outputs();
}

