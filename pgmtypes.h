#ifndef __PGMTYPES__
#define __PGMTYPES__

/* this is workaround for g++ bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734 */
#if (((__GNUC__ * 1000) + __GNUC_MINOR__) * 1000 + __GNUC_PATCHLEVEL__) < 4006002
# ifdef __cplusplus
#  undef PROGMEM
#  define PROGMEM __attribute__(( section(".progmem.data") ))
#  undef PSTR
# endif
#endif

/* old (pre 1.8.0) avr-libc does not have progmem macros compatible with new gcc 
 * note: even for old gcc, definition was wrong. Old gcc was just more tolerant. */
#if (((__AVR_LIBC_MAJOR__ * 1000) + __AVR_LIBC_MINOR__) * 1000 + __AVR_LIBC_REVISION__) < 1008000
# ifndef __PGMSPACE_H_
#  warning "This file should be included after <avr/pgmspace.h>"
# endif
# undef PSTR
# undef PGM_P
# undef PGM_VOID_P
# define PGM_P const char *
# define PGM_VOID_P const void *
#endif

#ifndef PSTR 
# define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s);&__c[0];}))
#endif

typedef void      pm_void;
typedef char      pm_char;
typedef unsigned char pm_uchar;
typedef int8_t    pm_int8_t;
typedef uint8_t   pm_uint8_t;
typedef int16_t   pm_int16_t;
typedef uint16_t  pm_uint16_t;
typedef int32_t   pm_int32_t;
typedef uint32_t  pm_uint32_t;

#endif /* __PGMTYPES__ */

