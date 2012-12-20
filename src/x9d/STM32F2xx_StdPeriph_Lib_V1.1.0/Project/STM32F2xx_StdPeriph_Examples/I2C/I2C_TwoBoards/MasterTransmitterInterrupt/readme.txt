/**
  @page I2C_MasterTransmitterInterrupt I2C Communication between Master in mode transmitter
  and slave in mode Receiver using interrupts example
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    I2C/I2C_TwoBoards/MasterTransmitterInterrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the Two boards I2C Communication example: Master transmitter
  *          and Slave and receiver.
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
to trigger I2C communication using interrupt and though using I2C firmware library.

- Hardware Description

To use this example, you need to load it in two STM32 boards (let's call them 
Board A and Board B) then connect these two boards through I2C lines and GND.
In the firmware example uncomment the dedicated line in the main.h file  to use
the I2C peripheral as STM32 Master device or as STM32 Slave .

@verbatim
*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |I2C Master|____|_______SDA_______|____|I2C Slave |    |         |
|        |    |  Device  |____|_______SCL_______|____|  Device  |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |  O LD1             |                 |  O LD1             |         |
|        |  O LD2   Joystick  |                 |  O LD2             |         |
|        |  O LD3        _    |                 |  O LD3             |         |
|        |  O LD4       |_|   |                 |  O LD4             |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

- Software Description

This example requires two boards(Master and Slave)

In master board, I2C peripheral is configured as Master Transmitter with Interrupt,
whereas in Slave board, I2C peripheral is configured as Slave Receiver with Interrupt.
According to the pushed joystick button in the master board a specific LED will 
be ON in the slave board.

At each joystick position press
 - The master sends the defined NumberOfByte from TxBuffer to the slave 
 - The Slave compares the NumberOfByte received data with the defined ones into TxBuffer

Received data correctness is signaled by LED lightening and though as follow:
   - Joystick RIGHT and data correctly received ==> LD2, LD3 ON and LD4 OFF 
   - Joystick LEFT and data correctly received  ==> LD4 ON, LD2 and LD3 are OFF
   - Joystick UP and data correctly received    ==> LD2 ON, LD3 and LD4 are OFF
   - Joystick DOWN and data correctly received  ==> LD3 ON, LD2 and LD4 are OFF
   - Joystick SEL and data correctly received   ==> LD2, LD3 and LD4 are ON
   
Communication error are handled in both Master and Slave sides, their occurrence
will be signaled respectively by puting LED3 and LED4 On.
 
The File "I2C/I2C_TwoBoards/MasterTransmitterInterrupt/main.h" allows different 
possibilities of configuration of this example:
  - Addressing mode : 7-bit or 10-bit
  - I2C speed: Fast mode or Standard mode
  - Buffer size also can be configured
  
In both boards, the data transfers is managed using I2Cx_EV_IRQHandler 
and I2Cx_ER_IRQHandler in stm32f2xx_it.c file.

The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle the LD1 each 100ms indicating that the 
firmware is running.
These operations can be repeated infinitely.

A defined communication timeout is insuring that the application will not remain 
stuck if the I2C Master communication is corrupted.
You can adjust this timeout through the USER_TIMEOUT  define inside main.h file
depending on CPU frequency and application conditions (interrupts routines, 
number of data to transfer, speed, CPU frequency...).
When a given operation is not finished within the defined timeout, a callback is 
made to the function "TimeOut_UserCallback()", this function is used to manage
timeout communication failure. In this example, this callback implements a simple 
infinite loop, but user can add his own implementation to manage timeout failure.


@par Directory contents 

  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/stm32f2xx_conf.h    Library Configuration file
  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/stm32f2xx_it.c      Interrupt handlers
  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/stm32f2xx_it.h      Interrupt handlers header file
  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/main.c              Main program
  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/main.h              Main Header file
  - I2C/I2C_TwoBoards/MasterTransmitterInterrupt/system_stm32f2xx.c  STM32F2xx system source file
  
@note The "system_stm32f2xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F2xx_Clock_Configuration_V1.0.0.xls" 
      provided with the AN3362 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>
 
      
@par Hardware and Software environment 

  - This example runs on STM32F2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB (with no additional
    hardware), and can be easily tailored to any other development board.

  - STM322xG-EVAL Set-up 
    - Connect I2C1 Master SCL pin (PB.06) to I2C1 Slave SCL (PB.06)
    - Connect I2C1 Master SDA pin (PB.09) to I2C1 Slave SDA (PB.09)

   @note  
    - Pull-up resistors should be connected to I2C SCL and SDA Pins in case of 
      other boards use.
      Pull-up  resistors are already implemented on the STM322xG-EVAL evaluation 
      boards. 
      
@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F2xx_StdPeriph_Template
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


