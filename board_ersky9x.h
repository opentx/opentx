#include "ersky9x/AT91SAM3S2.h"

#define PIN_ENABLE           0x001
#define PIN_PERIPHERAL       0x000
#define PIN_INPUT            0x002
#define PIN_OUTPUT           0x000
#define PIN_PULLUP           0x004
#define PIN_NO_PULLUP        0x000
#define PIN_PULLDOWN         0x008
#define PIN_NO_PULLDOWN      0x000
#define PIN_PERI_MASK_L      0x010
#define PIN_PERI_MASK_H      0x020
#define PIN_PER_A            0x000
#define PIN_PER_B            0x010
#define PIN_PER_C            0x020
#define PIN_PER_D            0x030
#define PIN_PORT_MASK        0x0C0
#define PIN_PORTA            0x000
#define PIN_PORTB            0x040
#define PIN_PORTC            0x080
#define PIN_LOW              0x000
#define PIN_HIGH             0x100

void configure_pins( uint32_t pins, uint16_t config );
