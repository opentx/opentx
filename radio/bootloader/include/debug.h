#include <inttypes.h>

#define TRACE_DEBUG(...) { }
#define TRACE_DEBUG_WP(...) { }
#define TRACE_INFO(...) { }
#define TRACE_INFO_WP(...) { }
#define TRACE_WARNING(...) { }
#define TRACE_WARNING_WP(...) { }
#define TRACE_ERROR(...) { }

extern void handle_serial( void* pdata ) ;
//extern uint32_t read_switch( enum EnumKeys enuk ) ;
extern void txmit( uint8_t c ) ;
//extern void disp_mem( register uint32_t address ) ;
//extern void disp_256( uint32_t address, uint32_t lines ) ;
#ifdef __cplusplus
extern "C" void utxmit( uint8_t c ) ;
extern "C" void up8hex( uint32_t value ) ;
#else
extern void utxmit( uint8_t c ) ;
extern void up8hex( uint32_t value ) ;
#endif



