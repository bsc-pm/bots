
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

#define BOTS_ERROR                         0
#define BOTS_ERROR_NOT_ENOUGH_MEMORY       1
#define BOTS_ERROR_UNRECOGNIZED_PARAMETER  2

#define BOTS_WARNING                       0

void bots_get_date(char *str);
void bots_get_architecture(char *str);
void bots_get_load_average(char *str);
void bots_error(int error, char *message);
void bots_warning(int warning, char *message);
void bots_print_results(void);
#endif
