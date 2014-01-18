

#include <stdint.h>

#ifdef PCBSKY
#define wdt_reset()	(WDT->WDT_CR = 0xA5000001)
#endif
#ifdef PCBX9D
#define wdt_reset()	(IWDG->KR = 0x0000AAAAL)
#endif

#define POWER_OFF			0
#define POWER_ON			1

#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

#define BITMASK(bit) (1<<(bit))

#define _MSK_KEY_REPT    0x40
#define _MSK_KEY_DBL     0x10
#define IS_KEY_BREAK(key)  (((key)&0xf0)        ==  0x20)
#define EVT_KEY_BREAK(key) ((key)|                  0x20)
#define EVT_KEY_FIRST(key) ((key)|    _MSK_KEY_REPT|0x20)
#define EVT_KEY_REPT(key)  ((key)|    _MSK_KEY_REPT     )
#define EVT_KEY_LONG(key)  ((key)|0x80)
#define EVT_KEY_DBL(key)   ((key)|_MSK_KEY_DBL)
//#define EVT_KEY_DBL(key)   ((key)|0x10)
#define EVT_ENTRY               (0xff - _MSK_KEY_REPT)
#define EVT_ENTRY_UP            (0xfe - _MSK_KEY_REPT)
#define EVT_KEY_MASK             0x0f


#define NUM_KEYS 6

#ifdef PCBSKY
enum EnumKeys {
    KEY_MENU ,
    KEY_EXIT ,
    KEY_DOWN ,
    KEY_UP  ,
    KEY_RIGHT ,
    KEY_LEFT
} ;
#endif

#ifdef PCBX9D
enum EnumKeys {
    KEY_MENU ,
    KEY_EXIT ,
    KEY_ENTER ,
    KEY_PAGE ,
    KEY_PLUS ,
    KEY_MINUS
} ;
#endif

extern void init_SDcard( void ) ;
extern unsigned long Master_frequency ;

extern void init_soft_power( void ) ;
extern uint32_t check_soft_power( void ) ;
extern void soft_power_off( void ) ;


