
#ifndef _COMMON_H
#define _COMMON_H

#ifndef CC
#define CC ""
#endif
#ifndef CFLAGS
#define CFLAGS ""
#endif
#ifndef LD
#define LD ""
#endif
#ifndef LDFLAGS
#define LDFLAGS ""
#endif
#ifndef CDATE
#define CDATE ""
#endif
#ifndef CMESSAGE
#define CMESSAGE ""
#endif

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif

#define NBS_ERROR                         0
#define NBS_ERROR_NOT_ENOUGH_MEMORY       1
#define NBS_ERROR_UNRECOGNIZED_PARAMETER  2

#define NBS_WARNING                       0


void nbs_get_date(char *str);
void nbs_get_architecture(char *str);
int nbs_get_max_cpus(void);
void nbs_get_load_average(char *str);
long nbs_usecs();
void nbs_error(int error, char *message);
void nbs_warning(int warning, char *message);

void nbs_print_results(void);
void nbs_print_results_row(void);
void nbs_print_header_row(void);
#endif
