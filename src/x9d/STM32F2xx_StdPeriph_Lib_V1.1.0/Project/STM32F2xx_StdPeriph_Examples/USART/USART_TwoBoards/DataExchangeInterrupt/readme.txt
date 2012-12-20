/**
  @page USART_DataExchangeInterrupt USART Communication Boards Data Exchange using Interrupt example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    USART/USART_TwoBoards/DataExchangeInterrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the USART Communication Boards Interrupt Data Exchange 
  *          using Interrupt example.
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
to trigger USART communication using interrupts and though using USART firmware 
library.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
BoardA and BoardB) then connect these two boards through UART lines and GND.

@verbatim
*------------------------------------------------------------------------------*
|                 BoardA                                BoardB                 |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |   UART   |____|TX_____________RX|____|   UART   |    |         |
|        |    |  Device1 |____|RX_____________TX|____|  Device2 |    |         |
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

@note
- The connection between the pins should use a common Ground.

- Software Description

On BoardA, at each joystick buttons press:
- The USART BoardA sends the specific command to the USART BoardB (the command 
  contains the transaction code (CMD_RIGHT, CMD_LEFT, CMD_UP, CMD_DOWN or CMD_SEL)
  followed by the number of data to be transmitted (CMD_RIGHT_SIZE, CMD_LEFT_SIZE, 
  CMD_UP_SIZE, CMD_DOWN_SIZE or CMD_SEL_SIZE)

- The USART BoardB receives the command and sends the CMD_ACK command to the USART BoardA

- The USART BoardA receives the CMD_ACK command and sends the number of bytes 
  from TxBuffer to the USART BoardB.

- The USART BoardB compares the number of bytes received with the defined ones into
  his TxBuffer.
  
  Received data correctness is signaled by LED lightening and though as follow:
   - Joystick RIGHT and data correctly received ==> LD2, LD3 ON and LD4 OFF 
   - Joystick LEFT and data correctly received  ==> LD4 ON, LD2 and LD3 are OFF
   - Joystick UP and data correctly received    ==> LD2 ON, LD3 and LD4 are OFF
   - Joystick DOWN and data correctly received  ==> LD3 ON, LD2 and LD4 are OFF
   - Joystick SEL and data correctly received   ==> LD2, LD3 and LD4 are ON

The steps described above can be also initiated and ensured by BoardB. 

In both boards(BoardA or BoardB), the data transfers is managed using USARTx_IRQHandler 
in stm32f2xx_it.c file.

The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle the LD1 each 100ms indicating that the 
firmware is running.
These operations can be repeated infinitely.

A defined communication timeout is insuring that the application will not remain 
stuck if the USART communication is corrupted.
You can adjust this timeout through the USER_TIMEOUT  define inside main.h file
depending on CPU frequency and application conditions (interrupts routines, 
number of data to transfer, baudrate, CPU frequency...).
When a given operation is not finished within the defined timeout, a callback is 
made to the function "TimeOut_UserCallback()", this function is used to manage
timeout communication failure. In this example, this callback implements a simple 
infinite loop, but user can add his own implementation to manage timeout failure.
   

@par Directory contents 

  - USART/USART_TwoBoards/DataExchangeInterrupt/stm32f2xx_conf.h    Library Configuration file
  - USART/USART_TwoBoards/DataExchangeInterrupt/stm32f2xx_it.c      Interrupt handlers
  - USART/USART_TwoBoards/DataExchangeInterrupt/stm32f2xx_it.h      Interrupt handlers header file
  - USART/USART_TwoBoards/DataExchangeInterrupt/main.c              Main program
  - USART/USART_TwoBoards/DataExchangeInterrupt/main.h              Main program header file
  - USART/USART_TwoBoards/DataExchangeInterrupt/system_stm32f2xx.c  STM32F2xx system source file
  
@note The "system_stm32f2xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F2xx_Clock_Configuration_V1.0.0.xls" 
      provided with the AN3362 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>


@par Hardware and Software environment

  - This example runs on STM32F2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB and can be easily tailored
    to any other development board.

  - STM322xG-EVAL Set-up
    - Use LED1, LED2, LED3 and LED4 connected respectively to PG.06, PG.08, PI.09
      and PC.07 pins
    - Use the Joystick buttons 
    - Connect BoardA USART3 TX pin (PC.10) to BoardB USART3 RX pin (PC.11)
    - Connect BoardA USART3 RX pin (PC.11) to BoardB USART3 TX pin (PC.10)

  
@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F2xx_StdPeriph_Template
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


