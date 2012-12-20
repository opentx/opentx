/**
  @page SPI_DataExchangeInterrupt SPI Communication Boards Data Exchange using interrupt example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    SPI/SPI_TwoBoards/DataExchangeInterrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the SPI Communication Boards Data Exchange using 
  *          interrupt example.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
   @endverbatim

@par Example Description 

This example provides a small application in which joystick buttons are used 
to trigger SPI communication using interrupts and though using SPI firmware 
library.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
BoardA and BoardB) then connect these two boards through SPI lines and GND.
In the firmware example uncomment the dedicated line in the main.h file to use
the SPI peripheral as STM32 Master device or as STM32 Slave .

@verbatim
*------------------------------------------------------------------------------*
|                BoardA                                 BoardB                 |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |SPI Master|____|_______SCK_______|____|SPI Slave |    |         |
|        |    |  Device  |____|_______MOSI______|____|  Device  |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |                    |                 |                    |         |
|        |  O LD1             |                 |  O LD1             |         |
|        |  O LD2    Joystick |                 |  O LD2    Joystick |         |
|        |  O LD3        _    |                 |  O LD3        _    |         |
|        |  O LD4       |_|   |                 |  O LD4       |_|   |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

@note The connection between the pins should use a short wires and a common Ground.

- Software Description
In master board, SPI peripheral is configured as Master full duplex with Interrupt,
whereas in Slave board, SPI peripheral is configured as Slave Full duplex with Interrupt. 

At each joystick buttons press:
- The Master sends a specific command to the Slave, the command contains the 
  transaction code (CMD_RIGHT, CMD_LEFT, CMD_UP, CMD_DOWN or CMD_SEL), then 
  the defined NumberOfByte from TxBuffer.

   
- The Slave after checking the received command, compares the NumberOfByte received data 
  with the defined ones into TxBuffer. Received data correctness is signaled by LED 
  lightening and though as follow:
   - Received command CMD_RIGHT and data correctly received --> LD2 and LD3 ON
   - Received command CMD_LEFT and data correctly received  --> LD4 ON
   - Received command CMD_UP and data correctly received    --> LD2 ON
   - Received command CMD_DOWN and data correctly received  --> LD3 ON
   - Received command CMD_SEL and data correctly received   --> LD2, LD3 and LD4 ON

In both boards, the data transfers is managed using SPIx_IRQHandler in stm32f2xx_it.c 
file.
The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle LD1 each 100ms indicating that the 
firmware is running.
These operations can be repeated infinitely.


A defined communication timeout is insuring that the application will not remain
stuck if the SPI communication is corrupted.
You can adjust this timeout through the USER_TIMEOUT define inside main.h file
depending on CPU frequency and application conditions (interrupts routines, 
number of data to transfer, baudrate, CPU frequency...).
When a given operation is not finished within the defined timeout, a callback is 
made to the function "TimeOut_UserCallback()", this function is used to manage
timeout communication failure. In this example, this callback implements a simple 
infinite loop, but user can add his own implementation to manage timeout failure.


@par Directory contents 

  - SPI/SPI_TwoBoards/DataExchangeInterrupt/stm32f2xx_conf.h    Library Configuration file
  - SPI/SPI_TwoBoards/DataExchangeInterrupt/stm32f2xx_it.c      Interrupt handlers
  - SPI/SPI_TwoBoards/DataExchangeInterrupt/stm32f2xx_it.h      Interrupt handlers header file
  - SPI/SPI_TwoBoards/DataExchangeInterrupt/main.c              Main program
  - SPI/SPI_TwoBoards/DataExchangeInterrupt/main.h              Main program header file
  - SPI/SPI_TwoBoards/DataExchangeInterrupt/system_stm32f2xx.c  STM32F2xx system source file
  
@note The "system_stm32f2xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F2xx_Clock_Configuration_V1.0.0.xls" tool. 

 
@par Hardware and Software environment

  - This example runs on STM32F2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB (with no additional
    hardware), and can be easily tailored to any other development board.

  - STM322xG-EVAL Set-up 
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins
    - Use the Joystick Push Button 
    - Connect BoardA SPI2 SCK pin  (PI1) to BoardB SPI2 SCK pin  (PI1)
    - Connect BoardA SPI2 MOSI pin (PI3) to BoardB SPI2 MOSI pin (PI3)


@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F2xx_StdPeriph_Template
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


