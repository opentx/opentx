
#include "../../opentx.h"
#include "usb_joystick.h"


/*
  Prepare and send new USB data packet

  The format of HID_Buffer is defined by
  USB endpoint description can be found in 
  file usb_hid_joystick.c, variable HID_JOYSTICK_ReportDesc
*/
void usb_joystick_update(void)
{
  static uint8_t HID_Buffer[HID_IN_PACKET];
  
  pauseMixerCalculations();

  //buttons
  HID_Buffer[0] = 0; //buttons
  for (int i = 0; i < 8; ++i) {
    if ( channelOutputs[i+8] > 0 ) {
      HID_Buffer[0] |= (1 << i);
    } 
  }

  //analog values
  //uint8_t * p = HID_Buffer + 1;
  for (int i = 0; i < 8; ++i) {
    int16_t value = channelOutputs[i] / 8;
    if ( value > 127 ) value = 127;
    else if ( value < -127 ) value = -127;
    HID_Buffer[i+1] = static_cast<int8_t>(value);  
    //*p++ = (channelOutputs[i] + 2000) & 0xFF;        //LSB
    //*p++ = ((channelOutputs[i] + 2000) >> 8) & 0xFF;  //MSB
  }

  resumeMixerCalculations();
  USBD_HID_SendReport (&USB_OTG_dev, HID_Buffer, HID_IN_PACKET );
}
