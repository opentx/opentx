## OpenTX 2.3 Branch

[![Travis build Status](https://travis-ci.org/opentx/opentx.svg?branch=2.3)](https://travis-ci.org/opentx/opentx)
[![Join the chat at https://chat.open-tx.org](https://camo.githubusercontent.com/3d659054abd6ce21c0e47cf3b83a51bda69ca282/68747470733a2f2f64656d6f2e726f636b65742e636861742f696d616765732f6a6f696e2d636861742e737667)](https://chat.open-tx.org)
[![Donate using Paypal](https://img.shields.io/badge/paypal-donate-yellow.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DJ9MASSKVW8WN)

The ongoing development on 2.3.x is done in this branch.

Refer to the [OpenTX wiki](https://github.com/opentx/opentx/wiki) for information about setting up the tool chains for building OpenTX and OpenTX Companion as well as other development related issues.

Useful links:
 * OpenTX Main Site https://www.open-tx.org/
 * OpenTx University http://open-txu.org/
 * OpenTX User Manual https://www.gitbook.com/book/opentx/opentx-taranis-manual/details
 * OpenTX Lua Reference Guide https://www.gitbook.com/book/opentx/opentx-lua-reference-guide/details

## Purpose of this branch

Enable HAL (Haerdware Abstraction Layer for the STM32F407VET6 chip on the popular STM32FAVE development board. Where this is motivated for the following reasons: 

* To have a cheap development platform to test on.
* To have a cheap alaternative to DIY radio platform. (Endless possibilities of design) 
* To be able to add periferals as needed to test different connectivity - (Like 4in1, New radio protocol, sticks, ect... ) 

With in this brach, I hope to provide support for the following cheap LCD screens to see the OpenTX display. 

* 3.2 inch TFT LCD screen with resistive touch screens ILI9341 display module for STM32 
* 5" Graphical LCD Touchscreen, 480x272, SPI, FT800

## More on the STM32F407VET6 and STM32FAVE development Board.
![alt text](https://github.com/jemo07/opentx/blob/2.3/stm32407vet6_view01.jpg)

# Microcontroller features
* STM32F407VET6 in LQFP100 package
* ARM®32-bit Cortex®-M4 CPU + FPU
* 168 MHz max CPU frequency
* VDD from 1.8 V to 3.6 V
* 512 KB Flash
* 192+4 Kbytes of SRAM including 64-Kbyte of CCM (core coupled memory) data RAM
* GPIO (82) with external interrupt capability
* 12-bit ADC (3) with 16 channels
* 12-bit DAC (1) with 2 channels
* RTC
* Timers (14)
* I2C (3) interfaces (SMBus/PMBus)
* I2S (2)
* USART (4)
* SPI (3)
* USB 2.0 full-speed
* USB 2.0 OTG
* CAN (2)

# Board features
* JTAG/SWD header
* Micro SD
* Winbond W25Q16 16Mbit SPI Flash
* RTC battery CR1220
* 10/100 Ethernet MAC
* 3.3V LDO voltage regulator
* Mini USB connector
* 1x power LED
* 2x user LEDs D2 (PA6) D3 (PA7)
* 2x jumpers for bootloader selection
* Reset button,
* Wakeup button
* 2x user buttons K0 (PE4) and K1 (PE3)
* 2x24 side pins + 2x16 bottom pins + 1x4 ISP pins
* 2x16 FMSC LCD Interface
* NRF24L01 socket 

# Board pinout
![alt text](https://github.com/jemo07/opentx/blob/2.3/stm32f407vet6_left02.png)
![alt text](https://github.com/jemo07/opentx/blob/2.3/stm32f407vet6_right01.png)
![alt text](https://github.com/jemo07/opentx/blob/2.3/stm32f407vet6_st-link02.png)




