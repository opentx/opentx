#ifndef lpc2000_debug_printf_h_
#define lpc2000_debug_printf_h_

extern void lpc2000_debug_devopen( int(*put)(int) );
extern void lpc2000_debug_printf(char const *format, ...);

#endif
