/**
  @page DCMI_Camera Camera example Readme file
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    DCMI/Camera/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Description of the stm32f2xx camera example.
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

@par Description

This example shows how to use the DCMI to control the OV9655 or OV2640 Camera module
mounted on STM322xG-EVAL board RevB. 

Both cameras have an image array capable of operating at up to 15 frames per second 
in SXGA resolution with complete user control over image quality, formatting and
output data transfer. All required image processing functions are also programmable
through the SCCB interface (I2C like protocol).

In this example the DCMI is configured to interface with this 8 bit data camera
in continuous mode. The I2C1 is used to configure the Camera in 8 bit RGB 5:6:5 mode. 
The user can select between two resolutions QQVGA(160x120) or QVGA(320x240) in order 
to display the captured image on the LCD(320x240), this selection is performed 
at application run time using the Joystick UP, DOWN and SEL buttons.
All camera data received by the DCMI are transferred through the DMA and displayed
on the LCD (connected to FSMC). As a result the CPU is free to execute other tasks.

You can use the potentiometer RV1 to fine tune on the fly the camera's brightness.
       

@par Project Directory contents

    - DCMI/Camera/main.h               Main config file
    - DCMI/Camera/camera_api.h         Header for camera_api.c
    - DCMI/Camera/dcmi_ov2640.h        Header for dcmi_ov2640.c
    - DCMI/Camera/dcmi_ov9655.h        Header for dcmi_ov9655.c
    - DCMI/Camera/lcd_lo_conf.h        Lcd_log configuration file
    - DCMI/Camera/stm32f2xx_it.h       Header for stm32f2xx_it.c
    - DCMI/Camera/stm32f2xx_conf.h     library configuration file 
    - DCMI/Camera/main.c               Main program  
    - DCMI/Camera/camera_api.c         High level driver for OV9655/OV2640 Cameras
    - DCMI/Camera/dcmi_ov2640.c        Driver for OV2640 Camera module
    - DCMI/Camera/dcmi_ov9655.c        Driver for OV9655 Camera module
    - DCMI/Camera/stm32f2xx_it.c       Interrupt handlers
    - DCMI/Camera/system_stm32f2xx.c   stm32f2xx system clock configuration file


@par Hardware and Software environment  
  - This example runs on stm32f2xx Devices.
  
  - This example has been tested with STM322xG-EVAL RevB.


@par How to use it ?
In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\stm32f2xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Add the following files the project source list
     - camera_api.c
     - dcmi_ov2640.c
     - dcmi_ov9655.c
     - Utilities\STM32_EVAL\Common\lcd_log.c
 - Rebuild all files and load your image into target memory
 - Run the example

  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
