#ifndef lua_protect_h
#define lua_protect_h

#include <setjmp.h>

struct our_longjmp {
  struct our_longjmp *previous;
  jmp_buf b;
  volatile int status;  /* error code */
};


#define PROTECT_LUA()   { struct our_longjmp lj; \
                        lj.previous = global_lj;     \
                        global_lj = &lj;  \
                        if (setjmp(lj.b) == 0)

#define UNPROTECT_LUA() global_lj = lj.previous; } 

extern struct our_longjmp * global_lj;

#endif  // #ifndef lua_protect_h
