/**
  @page ride RIDE Project Template for STM32F2xx devices
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   This sub-directory contains all the user-modifiable files needed to
  *          create a new project linked with the STM32F2xx Standard Peripherals
  *          Library and working with RIDE7 software toolchain.
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
 
 @par Directory contents
 
 - project .rprj/.rapp: A pre-configured project file with the provided library 
                        structure that produces an executable image with RIDE7.

 - stm32f2xx_flash_extsram.ld: This file is the RAISONANCE linker script used to 
                               place program code (readonly) in internal FLASH and 
                               data (readwrite, Stack and Heap)in external SRAM. 
                               You can customize this file to your need.
                                                          
 @par How to use it ?
 
 - Open the Project.rprj project.                          
 - Rebuild all files: Project->build project
 - Load project image: Debug->start(ctrl+D)
 - Run program: Debug->Run(ctrl+F9)  

 @note The needed define symbols for this config are already declared in the
       preprocessor section: USE_STM322xG_EVAL, STM32F2XX, USE_STDPERIPH_DRIVER 


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */


