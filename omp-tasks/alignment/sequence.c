#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "alignment.h"
#include "bots.h"

extern int *seqlen_array;
extern int nseqs, gap_pos2;

extern char **args, **names, **seq_array, *amino_acid_codes;

/***********************************************************************
 * :
 **********************************************************************/
size_t strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
			break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';                /* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);        /* count does not include NUL */
}

/***********************************************************************
 * : 
 **********************************************************************/
void fill_chartab(char *chartab)
{
	int i;

	for (i = 0; i < 128; i++) chartab[i] = 0;

	for (i = 0; i < 25; i++) {
		char c = amino_acid_codes[i];
		chartab[(int)c] = chartab[tolower(c)] = c;
	}
}

/***********************************************************************
 * : 
 **********************************************************************/
void encode(char *seq, char *naseq, int l) 
{
	int i, j;
	char c, *t;

	for (i = 1; i <= l; i++)
		if (seq[i] == '-') {
			naseq[i] = gap_pos2;
		} else {
			j = 0;
			c = seq[i];
			t = amino_acid_codes;
			naseq[i] = -1;
			while (t[j]) {if (t[j] == c) {naseq[i] = j; break;} j++;}
		}

	naseq[l + 1] = -3;
}


/***********************************************************************
 * : 
 **********************************************************************/
void alloc_aln(int nseqs)
{
	int i;

	names        = (char   **) malloc((nseqs + 1) * sizeof(char *));
	seq_array    = (char   **) malloc((nseqs + 1) * sizeof(char *));
	seqlen_array = (int     *) malloc((nseqs + 1) * sizeof(int));

	for (i = 0; i < nseqs + 1; i++) {
		names[i]     = (char *  ) malloc((MAXNAMES + 1) * sizeof(char));
		seq_array[i] = NULL;
	}
}

/***********************************************************************
 * : 
 **********************************************************************/
char * get_seq(char *sname, int *len, char *chartab, FILE *fin)
{
	int  i, j;
	char c, *seq;
	static char line[MAXLINE+1];

	*len = 0;
	seq  = NULL;

	while (*line != '>') fgets(line, MAXLINE+1, fin);
	for (i = 1; i <= strlen(line); i++) if (line[i] != ' ') break;
	for (j = i; j <= strlen(line); j++) if (line[j] == ' ') break;

	strlcpy(sname, line + i, j - i + 1);;
	sname[j - i] = EOS;

	while (fgets(line, MAXLINE+1, fin)) {
		if (seq == NULL)
			seq = (char *) malloc((MAXLINE + 2) * sizeof(char));
		else
			seq = (char *) realloc(seq, ((*len) + MAXLINE + 2) * sizeof(char));
		for (i = 0; i <= MAXLINE; i++) {
			c = line[i];
			if (c == '\n' || c == EOS || c == '>') break;
			if (c == chartab[(int)c]) {*len += 1; seq[*len] = c;}
		}
		if (c == '>') break;
	}

	seq[*len + 1] = EOS;
	return seq;
}

int readseqs(int first_seq, char *filename)
{
	int  i, l1, no_seqs;
	FILE *fin;
	char *seq1, chartab[128];

	if ((fin = fopen(filename, "r")) == NULL) {
		fprintf(stdout, "Could not open sequence file (%s)\n", args[0]);
		exit (-1);
	}

	fscanf(fin,"Number of sequences is %d", &no_seqs);
	
	fill_chartab(chartab);
	if (bots_verbose_mode) fprintf(stdout, "Sequence format is Pearson\n");

	alloc_aln(no_seqs);

	for (i = 1; i <= no_seqs; i++) {
		seq1 = get_seq(names[i], &l1, chartab, fin);

		seqlen_array[i] = l1;
		seq_array[i]    = (char *) malloc((l1 + 2) * sizeof (char));

		encode(seq1, seq_array[i], l1);

		free(seq1);
	}

	return no_seqs;
}

