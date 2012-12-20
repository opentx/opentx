/**
  @page BKP_Domain RTC Backup domain example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    RTC/BKP_Domain/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the RTC Backup domain example.
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

This example demonstrates and explains how to use the peripherals available on
Backup Domain. These peripherals are: the RCC BDCR register containing the LSE Osc 
configuration and the RTC Clock enable/disable bits. It embeds also the RTC 
peripheral with its associated Backup Data registers. And finally the Backup SRAM (4KB)
with its low power regulator allowing to preserve its contents when the 
product is powered by VBAT pin.

As an application example, it demonstrates how to setup the RTC hardware calendar,
RTC Backup Data registers read/write and BKPSRAM read/write operations. 

One from the following clock can be used as RTC clock source (uncomment the
corresponding define in main.c): 
  - LSE oscillator clock usually delivered by a 32.768 kHz quartz.
  - LSI oscillator clock
  
The Backup (BKP) domain, still powered by VBAT when VDD is switched off, so its
content is not lost if a battery is connected to the VBAT pin. 
A key value is written in backup data register 0 to indicate if the Backup Domain
is already configured.

The program behaves as follows:

1. After startup the program checks the backup data register 0 value:
    - BKP_DR0 value not correct: (RTC_BKP_DR0 value is not correct or has not yet
      been programmed when the program is executed for the first time) the RTC is
      configured and the user is asked to set the time and date (entered on 
      TFT-LCD) and the RTC Backup Data registers and BKPSRAM are written.
    
    - BKP_DR0 value correct: this means that the RTC is configured and the time
      and date are displayed on LCD. The content of RTC Backup Data registers 
      and BKPSRAM is checked and the result is displayed on color LCD.

2. When an External Reset occurs the BKP domain is not reset.

3. When power on reset occurs:
    - If a battery is connected to the VBAT pin: the BKP domain is not reset and
      the RTC configuration is not lost.
      
    - If no battery is connected to the VBAT pin: the BKP domain is reset and the
      RTC configuration is lost.

4. When an RTC Wakeup event occurs the RTC Wakeup interrupt is generated and the
   current Time/Date is refreshed on LCD.
   
The example uses TFT-LCD and push-button to configure the RTC clock and display
the current calendar:
 
 - When the Backup domain is not yet configured:
    - The RCC BDCR Register value is displayed on LCD before and after the Backup
      domain configuration.
    - All RTC Backup Data Registers are written and checked.
    - All BKP SRAM content is written and checked.
    - User asked for RTC Calendar Configuration using the Tamper/Wakeup/Key buttons:
       - Use WAKEUP button to decrement the current value.
       - Use TAMPER button to increment the current value.
       - Use KEY button to validate the current value.
    - Once configured, the RTC Calendar is displayed on the LCD each second using
      the RTC Wakeup Interrupt.

 - When the Backup domain is already configured:
    - No need to reconfigure the RTC Clock and Calendar.
    - All RTC Backup Data Registers are checked.
    - All BKP SRAM content is checked. If the Low power regulator is enabled, the
      BKPSRAM content is not lost.
    - The RTC Calendar is displayed on the LCD each second using
      the RTC Wakeup Interrupt.


@par Directory contents 

  - RTC/BKP_Domain/system_stm32f2xx.c   STM32F2xx system clock configuration file
  - RTC/BKP_Domain/stm32f2xx_conf.h     Library Configuration file
  - RTC/BKP_Domain/stm32f2xx_it.c       Interrupt handlers
  - RTC/BKP_Domain/stm32f2xx_it.h       Header for stm32f2xx_it.c
  - RTC/BKP_Domain/main.c               Main program
  - RTC/BKP_Domain/main.h               Main header file
  - RTC/BKP_Domain/lcd_log_conf.h       lcd_log configuration file

@note The "system_stm32f2xx.c" is generated by an automatic clock configuration 
      tool and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F2xx_Clock_Configuration_V1.0.0.xls" 
      provided with the AN3362 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>

      
@par Hardware and Software environment 

  - This example runs on STM32F2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB and can be easily tailored
    to any other development board. 

  - STM322xG-EVAL Set-up
    - Use the TAMPER push button connected to PC.13 pin (EXTI Line13).
    - Use the KEY push button connected to PG.15 pin (EXTI Line15).
    - Use the WAKEUP push button connected to PA.00 pin (EXTI Line0).
    - All messages are displayed on the TFT-LCD.
    - Make sure that jumper JP19 is in position 2-3 to connect the 3V battery to VBAT pin


@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F2xx_StdPeriph_Template
 - Open your preferred toolchain
 - Add the file "lcd_log.c" (under Utilities\STM32_EVAL\Common) to the project source list
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


