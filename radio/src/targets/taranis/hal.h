/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _HAL_H_
#define _HAL_H_

// Keys
#if defined(PCBACAIR)
  #define KEYS_GPIO_REG_BIND            GPIOA->IDR
  #define KEYS_GPIO_PIN_BIND            GPIO_Pin_8  // PA.08
  #define KEYS_GPIO_REG_MENU            GPIOE->IDR
  #define KEYS_GPIO_PIN_MENU            GPIO_Pin_3  // PE.03
  #define KEYS_GPIO_REG_PAGE            GPIOE->IDR
  #define KEYS_GPIO_PIN_PAGE            GPIO_Pin_4  // PE.04
  #define KEYS_GPIO_REG_EXIT            GPIOE->IDR
  #define KEYS_GPIO_PIN_EXIT            GPIO_Pin_6  // PE.06
#elif defined(PCBX7)
  #define KEYS_GPIO_REG_MENU            GPIOD->IDR
  #define KEYS_GPIO_PIN_MENU            GPIO_Pin_7  // PD.07
  #define KEYS_GPIO_REG_PAGE            GPIOD->IDR
  #define KEYS_GPIO_PIN_PAGE            GPIO_Pin_3  // PD.03
  #define KEYS_GPIO_REG_EXIT            GPIOD->IDR
  #define KEYS_GPIO_PIN_EXIT            GPIO_Pin_2  // PD.02
#else
  #define KEYS_GPIO_REG_MENU            GPIOD->IDR
  #define KEYS_GPIO_PIN_MENU            GPIO_Pin_7  // PD.07
  #define KEYS_GPIO_REG_EXIT            GPIOD->IDR
  #define KEYS_GPIO_PIN_EXIT            GPIO_Pin_2  // PD.02
  #define KEYS_GPIO_REG_PAGE            GPIOD->IDR
  #define KEYS_GPIO_PIN_PAGE            GPIO_Pin_3  // PD.03
#endif

#if defined(PCBACAIR)
  #define KEYS_GPIO_REG_PLUS            GPIOC->IDR
  #define KEYS_GPIO_PIN_PLUS            GPIO_Pin_2  // PC.02
  #define KEYS_GPIO_REG_MINUS           GPIOC->IDR
  #define KEYS_GPIO_PIN_MINUS           GPIO_Pin_3  // PC.03
  #define KEYS_GPIO_REG_ENTER           GPIOE->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_5  // PE.05
#elif defined(PCBX9E)
  #define KEYS_GPIO_REG_ENTER           GPIOF->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_0  // PF.00
#elif defined(PCBX7)
  #define KEYS_GPIO_REG_ENTER           GPIOE->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_10 // PE.10
#else
  #define KEYS_GPIO_REG_PLUS            GPIOE->IDR
  #define KEYS_GPIO_PIN_PLUS            GPIO_Pin_10 // PE.10
  #define KEYS_GPIO_REG_MINUS           GPIOE->IDR
  #define KEYS_GPIO_PIN_MINUS           GPIO_Pin_11 // PE.11
  #define KEYS_GPIO_REG_ENTER           GPIOE->IDR
  #define KEYS_GPIO_PIN_ENTER           GPIO_Pin_12 // PE.12
#endif

// Rotary Encoder
#if defined(PCBX9E)
  #define ENC_GPIO                      GPIOD
  #define ENC_GPIO_PIN_A                GPIO_Pin_12 // PD.12
  #define ENC_GPIO_PIN_B                GPIO_Pin_13 // PD.13
  #define ROTARY_ENCODER_POSITION()     (ENC_GPIO->IDR >> 12) & 0x03
#elif defined(PCBACAIR)
  // TODO remove this
#elif defined(PCBX7)
  #define ENC_GPIO                      GPIOE
  #define ENC_GPIO_PIN_A                GPIO_Pin_9  // PE.09
  #define ENC_GPIO_PIN_B                GPIO_Pin_11 // PE.11
  #define ROTARY_ENCODER_POSITION()     (((ENC_GPIO->IDR >> 10) & 0x02) + ((ENC_GPIO->IDR >> 9) & 0x01))
#endif

// This is for SIMU: reuse rotary encoder pins to map UP and DOWN keyboard keys
#if defined(SIMU) && (defined(PCBX9E) || defined(PCBX7)) && !defined(PCBACAIR)
  #define KEYS_GPIO_REG_PLUS            ENC_GPIO->IDR
  #define KEYS_GPIO_PIN_PLUS            ENC_GPIO_PIN_A
  #define KEYS_GPIO_REG_MINUS           ENC_GPIO->IDR
  #define KEYS_GPIO_PIN_MINUS           ENC_GPIO_PIN_B
#endif

// Trims
#if defined(PCBX9E)
  #define TRIMS_GPIO_REG_LHL            GPIOG->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_1  // PG.01
  #define TRIMS_GPIO_REG_LHR            GPIOG->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_0  // PG.00
  #define TRIMS_GPIO_REG_LVD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_4  // PE.04
  #define TRIMS_GPIO_REG_LVU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_3  // PE.03
  #define TRIMS_GPIO_REG_RVD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_3  // PC.03
  #define TRIMS_GPIO_REG_RHL            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_1  // PC.01
  #define TRIMS_GPIO_REG_RVU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_2  // PC.02
  #define TRIMS_GPIO_REG_RHR            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_13 // PC.13
#elif defined(PCBACAIR)
  #define TRIMS_GPIO_REG_HLD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_HLD            GPIO_Pin_4  // PE.04
  #define TRIMS_GPIO_REG_HLU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_HLU            GPIO_Pin_3  // PE.03
  #define TRIMS_GPIO_REG_HMD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_HMD            GPIO_Pin_3  // PC.03
  #define TRIMS_GPIO_REG_HMU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_HMU            GPIO_Pin_2  // PC.02
  #define TRIMS_GPIO_REG_HHD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_HHD            GPIO_Pin_6  // PE.06
  #define TRIMS_GPIO_REG_HHU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_HHU            GPIO_Pin_5  // PE.05
  #define TRIMS_GPIO_REG_VLD            GPIOD->IDR
  #define TRIMS_GPIO_PIN_VLD            GPIO_Pin_14 // PD.14
  #define TRIMS_GPIO_REG_VLU            GPIOD->IDR
  #define TRIMS_GPIO_PIN_VLU            GPIO_Pin_15 // PD.15
  #define TRIMS_GPIO_REG_VMD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_VMD            GPIO_Pin_2  // PE.02
  #define TRIMS_GPIO_REG_VMU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_VMU            GPIO_Pin_1  // PE.01
  #define TRIMS_GPIO_REG_VHD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_VHD            GPIO_Pin_9  // PC.09
  #define TRIMS_GPIO_REG_VHU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_VHU            GPIO_Pin_8  // PC.08
#elif defined(PCBX7)
  #define TRIMS_GPIO_REG_LHL            GPIOD->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_15 // PD.15
  #define TRIMS_GPIO_REG_LHR            GPIOC->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_1  // PC.01
  #define TRIMS_GPIO_REG_LVD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_6  // PE.06
  #define TRIMS_GPIO_REG_LVU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_5  // PE.05
  #define TRIMS_GPIO_REG_RVD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_3  // PC.03
  #define TRIMS_GPIO_REG_RHL            GPIOE->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_3  // PE.03
  #define TRIMS_GPIO_REG_RVU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_2  // PC.02
  #define TRIMS_GPIO_REG_RHR            GPIOE->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_4  // PE.04
#else
  #define TRIMS_GPIO_REG_LHL            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LHL            GPIO_Pin_4  // PE.04
  #define TRIMS_GPIO_REG_LHR            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LHR            GPIO_Pin_3  // PE.03
  #define TRIMS_GPIO_REG_LVD            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVD            GPIO_Pin_6  // PE.06
  #define TRIMS_GPIO_REG_LVU            GPIOE->IDR
  #define TRIMS_GPIO_PIN_LVU            GPIO_Pin_5  // PE.05
  #define TRIMS_GPIO_REG_RVD            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVD            GPIO_Pin_3  // PC.03
  #define TRIMS_GPIO_REG_RHL            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RHL            GPIO_Pin_1  // PC.01
  #define TRIMS_GPIO_REG_RVU            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RVU            GPIO_Pin_2  // PC.02
  #define TRIMS_GPIO_REG_RHR            GPIOC->IDR
  #define TRIMS_GPIO_PIN_RHR            GPIO_Pin_13 // PC.13
#endif

// Switches
#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_A_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_A_H         GPIO_Pin_13 // PE.13
  #define SWITCHES_GPIO_REG_A_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_A_L         GPIO_Pin_7  // PE.07
#elif defined(PCBX7) || defined(PCBACAIR)
  #define SWITCHES_GPIO_REG_A_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_A_L         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_A_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_A_H         GPIO_Pin_13 // PE.13
#else
  #define SWITCHES_GPIO_REG_A_H         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_A_H         GPIO_Pin_5  // PB.05
  #define SWITCHES_GPIO_REG_A_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_A_L         GPIO_Pin_0  // PE.00
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_B_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_B_H         GPIO_Pin_11 // PG.11
  #define SWITCHES_GPIO_REG_B_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_B_L         GPIO_Pin_10 // PG.10
#elif defined(PCBX7) || defined(PCBACAIR)
  #define SWITCHES_GPIO_REG_B_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_B_L         GPIO_Pin_15 // PE.15
  #define SWITCHES_GPIO_REG_B_H         GPIOA->IDR
  #define SWITCHES_GPIO_PIN_B_H         GPIO_Pin_5  // PA.05
#else
  #define SWITCHES_GPIO_REG_B_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_B_H         GPIO_Pin_1  // PE.01
  #define SWITCHES_GPIO_REG_B_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_B_L         GPIO_Pin_2  // PE.02
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_C_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_C_H         GPIO_Pin_13 // PF.13
  #define SWITCHES_GPIO_REG_C_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_C_L         GPIO_Pin_14 // PF.14
#elif defined(PCBX7) || defined(PCBACAIR)
  #define SWITCHES_GPIO_REG_C_L         GPIOD->IDR
  #define SWITCHES_GPIO_PIN_C_L         GPIO_Pin_11 // PD.11
  #define SWITCHES_GPIO_REG_C_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_C_H         GPIO_Pin_0  // PE.00
#else
  #define SWITCHES_GPIO_REG_C_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_C_H         GPIO_Pin_15 // PE.15
  #define SWITCHES_GPIO_REG_C_L         GPIOA->IDR
  #define SWITCHES_GPIO_PIN_C_L         GPIO_Pin_5  // PA.05
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_D_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_1  // PE.01
  #define SWITCHES_GPIO_REG_D_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_2  // PE.02
#elif defined(PCBX9DP)
  #define SWITCHES_GPIO_REG_D_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_D_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_13 // PE.13
#elif defined(PCBX7)
  #define SWITCHES_GPIO_REG_D_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_1  // PE.01
  #define SWITCHES_GPIO_REG_D_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_2  // PE.02
#else
  #define SWITCHES_GPIO_REG_D_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_D_H         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_D_L         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_D_L         GPIO_Pin_1  // PB.01
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_E_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_E_H         GPIO_Pin_7  // PE.07
  #define SWITCHES_GPIO_REG_E_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_E_L         GPIO_Pin_13 // PE.13
#elif !defined(PCBX7)
  #define SWITCHES_GPIO_REG_E_H         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_E_H         GPIO_Pin_3  // PB.03
  #define SWITCHES_GPIO_REG_E_L         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_E_L         GPIO_Pin_4  // PB.04
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_F           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F           GPIO_Pin_11 // PE.11
#elif defined(PCBX7)
  #define SWITCHES_GPIO_REG_F           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F           GPIO_Pin_14 // PE.14
#else
  #define SWITCHES_GPIO_REG_F           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_F           GPIO_Pin_14 // PE.14
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_G_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_G_H         GPIO_Pin_3  // PF.03
  #define SWITCHES_GPIO_REG_G_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_G_L         GPIO_Pin_4  // PF.04
#elif !defined(PCBX7)
  #define SWITCHES_GPIO_REG_G_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_G_H         GPIO_Pin_9  // PE.09
  #define SWITCHES_GPIO_REG_G_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_G_L         GPIO_Pin_8  // PE.08
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_H           GPIOF->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_1  // PF.01
#elif defined(PCBX9DP)
  #define SWITCHES_GPIO_REG_H           GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_14 // PD.14
#elif defined(PCBX7)
  #define SWITCHES_GPIO_REG_H           GPIOD->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_14 // PD.14
#else
  #define SWITCHES_GPIO_REG_H           GPIOE->IDR
  #define SWITCHES_GPIO_PIN_H           GPIO_Pin_13 // PE.13
#endif

#if defined(PCBX9E)
  #define SWITCHES_GPIO_REG_I_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_I_H         GPIO_Pin_15 // PF.15
  #define SWITCHES_GPIO_REG_I_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_I_L         GPIO_Pin_14 // PE.14
  #define SWITCHES_GPIO_REG_J_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_J_H         GPIO_Pin_7  // PG.07
  #define SWITCHES_GPIO_REG_J_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_J_L         GPIO_Pin_8  // PG.08
  #define SWITCHES_GPIO_REG_K_H         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_K_H         GPIO_Pin_13 // PG.13
  #define SWITCHES_GPIO_REG_K_L         GPIOG->IDR
  #define SWITCHES_GPIO_PIN_K_L         GPIO_Pin_12 // PG.12
  #define SWITCHES_GPIO_REG_L_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_L_H         GPIO_Pin_9  // PE.09
  #define SWITCHES_GPIO_REG_L_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_L_L         GPIO_Pin_8  // PE.08
  #define SWITCHES_GPIO_REG_M_H         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_M_H         GPIO_Pin_15 // PE.15
  #define SWITCHES_GPIO_REG_M_L         GPIOA->IDR
  #define SWITCHES_GPIO_PIN_M_L         GPIO_Pin_5  // PA.05
  #define SWITCHES_GPIO_REG_N_H         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_N_H         GPIO_Pin_3  // PB.03
  #define SWITCHES_GPIO_REG_N_L         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_N_L         GPIO_Pin_4  // PB.04
  #define SWITCHES_GPIO_REG_O_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_O_H         GPIO_Pin_7  // PF.07
  #define SWITCHES_GPIO_REG_O_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_O_L         GPIO_Pin_10 // PE.10
  #define SWITCHES_GPIO_REG_P_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_P_H         GPIO_Pin_11 // PF.11
  #define SWITCHES_GPIO_REG_P_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_P_L         GPIO_Pin_12 // PF.12
  #define SWITCHES_GPIO_REG_Q_H         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_Q_H         GPIO_Pin_5  // PF.05
  #define SWITCHES_GPIO_REG_Q_L         GPIOF->IDR
  #define SWITCHES_GPIO_PIN_Q_L         GPIO_Pin_6  // PF.06
  #define SWITCHES_GPIO_REG_R_H         GPIOB->IDR
  #define SWITCHES_GPIO_PIN_R_H         GPIO_Pin_5  // PB.05
  #define SWITCHES_GPIO_REG_R_L         GPIOE->IDR
  #define SWITCHES_GPIO_PIN_R_L         GPIO_Pin_0  // PE.00
#endif

#if defined(PCBX9E)
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG)
  #define KEYS_GPIOA_PINS               (SWITCHES_GPIO_PIN_M_L)
  #define KEYS_GPIOB_PINS               (SWITCHES_GPIO_PIN_N_H | SWITCHES_GPIO_PIN_N_L | SWITCHES_GPIO_PIN_R_H)
  #define KEYS_GPIOC_PINS               (TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR)
  #define KEYS_GPIOD_PINS               (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE | SWITCHES_GPIO_PIN_A_H | SWITCHES_GPIO_PIN_A_L | ENC_GPIO_PIN_A | ENC_GPIO_PIN_B)
  #define KEYS_GPIOE_PINS               (TRIMS_GPIO_PIN_LVU | TRIMS_GPIO_PIN_LVD | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_D_L| SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_E_L | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_I_L | SWITCHES_GPIO_PIN_L_H | SWITCHES_GPIO_PIN_L_L | SWITCHES_GPIO_PIN_M_H | SWITCHES_GPIO_PIN_O_L | SWITCHES_GPIO_PIN_R_L)
  #define KEYS_GPIOF_PINS               (KEYS_GPIO_PIN_ENTER | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_C_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H | SWITCHES_GPIO_PIN_I_H | SWITCHES_GPIO_PIN_O_H | SWITCHES_GPIO_PIN_P_H | SWITCHES_GPIO_PIN_P_L | SWITCHES_GPIO_PIN_Q_H | SWITCHES_GPIO_PIN_Q_L)
  #define KEYS_GPIOG_PINS               (TRIMS_GPIO_PIN_LHL | TRIMS_GPIO_PIN_LHR| SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_J_H | SWITCHES_GPIO_PIN_J_L | SWITCHES_GPIO_PIN_K_H | SWITCHES_GPIO_PIN_K_L)
#elif defined(PCBX9DP)
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
  #define KEYS_GPIOA_PINS               (SWITCHES_GPIO_PIN_C_L)
  #define KEYS_GPIOB_PINS               (SWITCHES_GPIO_PIN_E_L | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_A_H)
  #define KEYS_GPIOC_PINS               (TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR)
  #define KEYS_GPIOD_PINS               (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE | SWITCHES_GPIO_PIN_H)
  #define KEYS_GPIOE_PINS               (KEYS_GPIO_PIN_PLUS | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_MINUS | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_LHL | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L)
#elif defined(PCBACAIR)
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
  #define KEYS_GPIOA_PINS               (GPIO_Pin_5 | GPIO_Pin_8)
  #define KEYS_GPIOC_PINS               (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_9)
  #define KEYS_GPIOD_PINS               (GPIO_Pin_11 | GPIO_Pin_15 | GPIO_Pin_14)
  #define KEYS_GPIOE_PINS               (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_13 | GPIO_Pin_15)
#elif defined(PCBX7)
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
  #define KEYS_GPIOA_PINS               SWITCHES_GPIO_PIN_B_H
  #define KEYS_GPIOC_PINS               (TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU)
  #define KEYS_GPIOD_PINS               (TRIMS_GPIO_PIN_LHL | KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE | SWITCHES_GPIO_PIN_C_L | SWITCHES_GPIO_PIN_H)
  #define KEYS_GPIOE_PINS               (ENC_GPIO_PIN_A | ENC_GPIO_PIN_B | KEYS_GPIO_PIN_ENTER | TRIMS_GPIO_PIN_RHR | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_L | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_A_H | SWITCHES_GPIO_PIN_F)
#else
  #define KEYS_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)
  #define KEYS_GPIOA_PINS               (SWITCHES_GPIO_PIN_C_L)
  #define KEYS_GPIOB_PINS               (SWITCHES_GPIO_PIN_E_L | SWITCHES_GPIO_PIN_E_H | SWITCHES_GPIO_PIN_A_H | SWITCHES_GPIO_PIN_D_L)
  #define KEYS_GPIOC_PINS               (TRIMS_GPIO_PIN_RVD | TRIMS_GPIO_PIN_RVU | TRIMS_GPIO_PIN_RHL | TRIMS_GPIO_PIN_RHR)
  #define KEYS_GPIOD_PINS               (KEYS_GPIO_PIN_MENU | KEYS_GPIO_PIN_EXIT | KEYS_GPIO_PIN_PAGE)
  #define KEYS_GPIOE_PINS               (KEYS_GPIO_PIN_PLUS | KEYS_GPIO_PIN_ENTER | KEYS_GPIO_PIN_MINUS | TRIMS_GPIO_PIN_LHR | TRIMS_GPIO_PIN_LHL | TRIMS_GPIO_PIN_LVD | TRIMS_GPIO_PIN_LVU | SWITCHES_GPIO_PIN_F | SWITCHES_GPIO_PIN_A_L | SWITCHES_GPIO_PIN_B_H | SWITCHES_GPIO_PIN_B_L | SWITCHES_GPIO_PIN_C_H | SWITCHES_GPIO_PIN_D_H | SWITCHES_GPIO_PIN_G_H | SWITCHES_GPIO_PIN_G_L | SWITCHES_GPIO_PIN_H)
#endif

// ADC
#if defined(PCBX9E)
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB2Periph            (RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC3)
#elif defined(PCBACAIR)
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB2Periph            RCC_APB2Periph_ADC1
#else
  #define ADC_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA2)
  #define ADC_RCC_APB2Periph            RCC_APB2Periph_ADC1
#endif
#define ADC_GPIO_PIN_STICK_RV           GPIO_Pin_0  // PA.00
#define ADC_GPIO_PIN_STICK_RH           GPIO_Pin_1  // PA.01
#define ADC_CHANNEL_STICK_RV            ADC_Channel_0  // ADC1_IN0
#define ADC_CHANNEL_STICK_RH            ADC_Channel_1  // ADC1_IN1
#if defined(PCBACAIR)
#elif defined(PCBX7)
  #define ADC_GPIO_PIN_STICK_LV         GPIO_Pin_2  // PA.02
  #define ADC_GPIO_PIN_STICK_LH         GPIO_Pin_3  // PA.03
  #define ADC_CHANNEL_STICK_LV          ADC_Channel_2  // ADC1_IN2
  #define ADC_CHANNEL_STICK_LH          ADC_Channel_3  // ADC1_IN3
#else
  #define ADC_GPIO_PIN_STICK_LH         GPIO_Pin_2  // PA.02
  #define ADC_GPIO_PIN_STICK_LV         GPIO_Pin_3  // PA.03
  #define ADC_CHANNEL_STICK_LH          ADC_Channel_2  // ADC1_IN2
  #define ADC_CHANNEL_STICK_LV          ADC_Channel_3  // ADC1_IN3
#endif
#define ADC_MAIN                        ADC1
#define ADC_DMA                         DMA2
#define ADC_DMA_SxCR_CHSEL              0
#define ADC_DMA_Stream                  DMA2_Stream4
#define ADC_SET_DMA_FLAGS()             ADC_DMA->HIFCR = (DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4)
#define ADC_TRANSFER_COMPLETE()         (ADC_DMA->HISR & DMA_HISR_TCIF4)
#define ADC_SAMPTIME                    2   // sample time = 28 cycles

#if defined(PCBX9E)
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_8  // PF.08
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_0  // PB.00
  #define ADC_GPIO_PIN_POT3             GPIO_Pin_5  // PC.05
  #define ADC_GPIO_PIN_POT4             GPIO_Pin_4  // PC.04
  #define ADC_GPIO_PIN_SLIDER1          GPIO_Pin_10 // PF.10
  #define ADC_GPIO_PIN_SLIDER2          GPIO_Pin_9  // PF.09
  #define ADC_GPIO_PIN_SLIDER3          GPIO_Pin_6  // PA.06
  #define ADC_GPIO_PIN_SLIDER4          GPIO_Pin_1  // PB.01
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS (ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_SLIDER3)
  #define ADC_GPIOB_PINS (ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_SLIDER4)
  #define ADC_GPIOC_PINS (ADC_GPIO_PIN_POT3 | ADC_GPIO_PIN_POT4 | ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT)
  #define ADC_GPIOF_PINS (ADC_GPIO_PIN_POT1 | ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2)
  #define ADC_CHANNEL_POT1              ADC_Channel_6  // ADC3_IN6
  #define ADC_CHANNEL_POT2              ADC_Channel_8  // ADC1_IN8
  #define ADC_CHANNEL_POT3              ADC_Channel_15 // ADC1_IN15
  #define ADC_CHANNEL_POT4              ADC_Channel_14 // ADC1_IN14
  #define ADC_CHANNEL_SLIDER1           ADC_Channel_8  // ADC3_IN8
  #define ADC_CHANNEL_SLIDER2           ADC_Channel_7  // ADC3_IN7
  #define ADC_CHANNEL_SLIDER3           ADC_Channel_6  // ADC1_IN6
  #define ADC_CHANNEL_SLIDER4           ADC_Channel_9  // ADC1_IN9
  #define ADC_CHANNEL_BATT              ADC_Channel_10 // ADC1_IN10
  #define ADC_EXT                       ADC3
  #define ADC_EXT_DMA                   DMA2
  #define ADC_EXT_DMA_Stream            DMA2_Stream0
  #define ADC_EXT_SET_DMA_FLAGS()       ADC_DMA->LIFCR = (DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0)
  #define ADC_EXT_TRANSFER_COMPLETE()   (ADC_DMA->LISR & DMA_LISR_TCIF0)
  #define ADC_EXT_SAMPTIME              3    // sample time = 56 cycles
#elif defined(PCBX9DP)
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_6  // PA.06
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_0  // PB.00
  #define ADC_GPIO_PIN_POT3             GPIO_Pin_1  // PB.01
  #define ADC_GPIO_PIN_SLIDER1          GPIO_Pin_4  // PC.04
  #define ADC_GPIO_PIN_SLIDER2          GPIO_Pin_5  // PC.05
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS                (ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT1)
  #define ADC_GPIOB_PINS                (ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_POT3)
  #define ADC_GPIOC_PINS                (ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT)
  #define ADC_CHANNEL_POT1              ADC_Channel_6
  #define ADC_CHANNEL_POT2              ADC_Channel_8
  #define ADC_CHANNEL_POT3              ADC_Channel_9
  #define ADC_CHANNEL_SLIDER1           ADC_Channel_14
  #define ADC_CHANNEL_SLIDER2           ADC_Channel_15
  #define ADC_CHANNEL_BATT              ADC_Channel_10
#elif defined(PCBACAIR)
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS                (GPIO_Pin_0 | GPIO_Pin_1)
  #define ADC_GPIOC_PINS                (GPIO_Pin_0)
  #define ADC_CHANNEL_BATT              ADC_Channel_10
#elif defined(PCBX7)
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_6  // PA.06
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_0  // PB.00
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS                (ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT1)
  #define ADC_GPIOB_PINS                ADC_GPIO_PIN_POT2
  #define ADC_GPIOC_PINS                ADC_GPIO_PIN_BATT
  #define ADC_CHANNEL_POT1              ADC_Channel_6
  #define ADC_CHANNEL_POT2              ADC_Channel_8
  #define ADC_CHANNEL_BATT              ADC_Channel_10
#else
  #define ADC_GPIO_PIN_POT1             GPIO_Pin_6  // PA.06
  #define ADC_GPIO_PIN_POT2             GPIO_Pin_0  // PB.00
  #define ADC_GPIO_PIN_SLIDER1          GPIO_Pin_4  // PC.04
  #define ADC_GPIO_PIN_SLIDER2          GPIO_Pin_5  // PC.05
  #define ADC_GPIO_PIN_BATT             GPIO_Pin_0  // PC.00
  #define ADC_GPIOA_PINS                (ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT1)
  #define ADC_GPIOB_PINS                (ADC_GPIO_PIN_POT2)
  #define ADC_GPIOC_PINS                (ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT)
  #define ADC_CHANNEL_POT1              ADC_Channel_6
  #define ADC_CHANNEL_POT2              ADC_Channel_8
  #define ADC_CHANNEL_POT3              ADC_Channel_9
  #define ADC_CHANNEL_SLIDER1           ADC_Channel_14
  #define ADC_CHANNEL_SLIDER2           ADC_Channel_15
  #define ADC_CHANNEL_BATT              ADC_Channel_10
#endif

// PWR and LED driver
#define PWR_RCC_AHB1Periph              (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD)
#if defined(PCBX9E) || defined(PCBX7) || defined(PCBACAIR)
#define PWR_PRESS_BUTTON
#endif
#define PWR_GPIO                        GPIOD
#define PWR_SWITCH_GPIO_PIN             GPIO_Pin_1  // PD.01
#define PWR_ON_GPIO_PIN                 GPIO_Pin_0  // PD.00
#if defined(PCBX7) || defined(PCBACAIR)
#define LED_GREEN_GPIO                  GPIOC
#define LED_GREEN_GPIO_PIN              GPIO_Pin_4  // PC.04
#define LED_RED_GPIO                    GPIOC
#define LED_RED_GPIO_PIN                GPIO_Pin_5  // PC.05
#define LED_BLUE_GPIO                   GPIOB
#define LED_BLUE_GPIO_PIN               GPIO_Pin_1  // PB.01
#endif

// Internal Module
#define INTMODULE_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2)
#define INTMODULE_RCC_APB2Periph        RCC_APB2Periph_TIM1
#if defined(PCBX9E) || defined(PCBX9DP) || defined(PCBX7) || defined(PCBACAIR)
  #define INTMODULE_PWR_GPIO            GPIOC
  #define INTMODULE_PWR_GPIO_PIN        GPIO_Pin_6  // PC.06
#else
  #define INTMODULE_PWR_GPIO            GPIOD
  #define INTMODULE_PWR_GPIO_PIN        GPIO_Pin_15 // PD.15
#endif
#define INTMODULE_PPM_GPIO              GPIOA
#define INTMODULE_PPM_GPIO_PIN          GPIO_Pin_10 // PA.10
#define INTMODULE_PPM_GPIO_PinSource    GPIO_PinSource10
#define INTMODULE_TIMER                 TIM1
#define INTMODULE_TIMER_CC_IRQn         TIM1_CC_IRQn
#define INTMODULE_TIMER_CC_IRQHandler   TIM1_CC_IRQHandler
#define INTMODULE_PPM_GPIO_AF           GPIO_AF_TIM1
#define INTMODULE_DMA_CHANNEL           DMA_Channel_6
#define INTMODULE_DMA_STREAM            DMA2_Stream5
#define INTMODULE_DMA_IRQn              DMA2_Stream5_IRQn
#define INTMODULE_DMA_IRQHandler        DMA2_Stream5_IRQHandler
#define INTMODULE_DMA_FLAG_TC           DMA_IT_TCIF5
#define INTMODULE_TIMER_FREQ            (PERI2_FREQUENCY * TIMER_MULT_APB2)

// External Module
#define EXTMODULE_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA2)
#define EXTMODULE_RCC_APB2Periph        RCC_APB2Periph_TIM8
#define EXTMODULE_PWR_GPIO              GPIOD
#define EXTMODULE_PWR_GPIO_PIN          GPIO_Pin_8  // PD.08
#define EXTMODULE_PPM_GPIO              GPIOA
#define EXTMODULE_PPM_GPIO_PIN          GPIO_Pin_7  // PA.07
#define EXTMODULE_PPM_GPIO_PinSource    GPIO_PinSource7
#define EXTMODULE_TIMER                 TIM8
#define EXTMODULE_PPM_GPIO_AF           GPIO_AF_TIM8
#define EXTMODULE_TIMER_CC_IRQn         TIM8_CC_IRQn
#define EXTMODULE_TIMER_CC_IRQHandler   TIM8_CC_IRQHandler
#define EXTMODULE_DMA_CHANNEL           DMA_Channel_7
#define EXTMODULE_DMA_STREAM            DMA2_Stream1
#define EXTMODULE_DMA_IRQn              DMA2_Stream1_IRQn
#define EXTMODULE_DMA_IRQHandler        DMA2_Stream1_IRQHandler
#define EXTMODULE_DMA_FLAG_TC           DMA_IT_TCIF1
#define EXTMODULE_TIMER_FREQ            (PERI2_FREQUENCY * TIMER_MULT_APB2)

// Trainer Port
#define TRAINER_RCC_AHB1Periph          (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC)
#define TRAINER_RCC_APB1Periph          RCC_APB1Periph_TIM3
#define TRAINER_GPIO                    GPIOC
#define TRAINER_IN_GPIO_PIN             GPIO_Pin_8  // PC.08
#define TRAINER_IN_GPIO_PinSource       GPIO_PinSource8
#define TRAINER_OUT_GPIO_PIN            GPIO_Pin_9  // PC.09
#define TRAINER_OUT_GPIO_PinSource      GPIO_PinSource9
#define TRAINER_DETECT_GPIO             GPIOA
#define TRAINER_DETECT_GPIO_PIN         GPIO_Pin_8  // PA.08
#define TRAINER_TIMER                   TIM3
#define TRAINER_TIMER_IRQn              TIM3_IRQn
#define TRAINER_GPIO_AF                 GPIO_AF_TIM3
#define TRAINER_DMA                     DMA1
#define TRAINER_DMA_CHANNEL             DMA_Channel_5
#define TRAINER_DMA_STREAM              DMA1_Stream2
#define TRAINER_DMA_IRQn                DMA1_Stream2_IRQn
#define TRAINER_DMA_IRQHandler          DMA1_Stream2_IRQHandler
#define TRAINER_DMA_FLAG_TC             DMA_IT_TCIF2
#define TRAINER_TIMER_IRQn              TIM3_IRQn
#define TRAINER_TIMER_IRQHandler        TIM3_IRQHandler
#define TRAINER_TIMER_FREQ              (PERI1_FREQUENCY * TIMER_MULT_APB1)

// Serial Port
#if defined(PCBX7)
#define SERIAL_RCC_AHB1Periph           0
#define SERIAL_RCC_APB1Periph           0
#else
#define SERIAL_RCC_AHB1Periph           (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
#define SERIAL_RCC_APB1Periph           RCC_APB1Periph_USART3
#define SERIAL_GPIO                     GPIOB
#define SERIAL_GPIO_PIN_TX              GPIO_Pin_10 // PB.10
#define SERIAL_GPIO_PIN_RX              GPIO_Pin_11 // PB.11
#define SERIAL_GPIO_PinSource_TX        GPIO_PinSource10
#define SERIAL_GPIO_PinSource_RX        GPIO_PinSource11
#define SERIAL_GPIO_AF                  GPIO_AF_USART3
#define SERIAL_USART                    USART3
#define SERIAL_USART_IRQHandler         USART3_IRQHandler
#define SERIAL_USART_IRQn               USART3_IRQn
#define SERIAL_DMA_Stream_RX            DMA1_Stream1
#define SERIAL_DMA_Channel_RX           DMA_Channel_4
#endif

// Telemetry
#define TELEMETRY_RCC_AHB1Periph        (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
#define TELEMETRY_RCC_APB1Periph        RCC_APB1Periph_USART2
#define TELEMETRY_DIR_GPIO              GPIOD
#define TELEMETRY_DIR_GPIO_PIN          GPIO_Pin_4  // PD.04
#define TELEMETRY_GPIO                  GPIOD
#define TELEMETRY_TX_GPIO_PIN           GPIO_Pin_5  // PD.05
#define TELEMETRY_RX_GPIO_PIN           GPIO_Pin_6  // PD.06
#define TELEMETRY_GPIO_PinSource_TX     GPIO_PinSource5
#define TELEMETRY_GPIO_PinSource_RX     GPIO_PinSource6
#define TELEMETRY_GPIO_AF               GPIO_AF_USART2
#define TELEMETRY_USART                 USART2
#define TELEMETRY_DMA_Stream_TX         DMA1_Stream6
#define TELEMETRY_DMA_Channel_TX        DMA_Channel_4
#define TELEMETRY_DMA_TX_Stream_IRQ     DMA1_Stream6_IRQn
#define TELEMETRY_DMA_TX_IRQHandler     DMA1_Stream6_IRQHandler
#define TELEMETRY_DMA_TX_FLAG_TC        DMA_IT_TCIF6
#define TELEMETRY_USART_IRQHandler      USART2_IRQHandler
#define TELEMETRY_USART_IRQn            USART2_IRQn

// PCBREV
#if defined(PCBX7)
#define PCBREV_RCC_AHB1Periph           RCC_AHB1Periph_GPIOA
#define PCBREV_GPIO                     GPIOA
#define PCBREV_GPIO_PIN                 GPIO_Pin_14  // PA.14
#else
#define PCBREV_RCC_AHB1Periph           0
#endif

// SPORT update connector
#if defined(PCBX7)
#define SPORT_UPDATE_RCC_AHB1Periph     RCC_AHB1Periph_GPIOB
#define SPORT_UPDATE_PWR_GPIO           GPIOB
#define SPORT_UPDATE_PWR_GPIO_PIN       GPIO_Pin_2  // PB.02
#else
#define SPORT_UPDATE_RCC_AHB1Periph     0
#endif

// Heartbeat
#define HEARTBEAT_RCC_AHB1Periph        RCC_AHB1Periph_GPIOC
#define HEARTBEAT_RCC_APB2Periph        RCC_APB2Periph_USART6
#define HEARTBEAT_GPIO                  GPIOC
#define HEARTBEAT_GPIO_PIN              GPIO_Pin_7  // PC.07
#define HEARTBEAT_GPIO_PinSource        GPIO_PinSource7
#define HEARTBEAT_GPIO_AF_SBUS          GPIO_AF_USART6
#define HEARTBEAT_GPIO_AF_CAPTURE       GPIO_AF_TIM3
#define HEARTBEAT_USART                 USART6
#define HEARTBEAT_USART_IRQHandler      USART6_IRQHandler
#define HEARTBEAT_USART_IRQn            USART6_IRQn
#define HEARTBEAT_DMA_Stream            DMA2_Stream1
#define HEARTBEAT_DMA_Channel           DMA_Channel_5

// USB
#define USB_RCC_AHB1Periph_GPIO         RCC_AHB1Periph_GPIOA
#define USB_GPIO                        GPIOA
#define USB_GPIO_PIN_VBUS               GPIO_Pin_9  // PA.09
#define USB_GPIO_PIN_DM                 GPIO_Pin_11 // PA.11
#define USB_GPIO_PIN_DP                 GPIO_Pin_12 // PA.12
#define USB_GPIO_PinSource_DM           GPIO_PinSource11
#define USB_GPIO_PinSource_DP           GPIO_PinSource12
#define USB_GPIO_AF                     GPIO_AF_OTG1_FS

// BackLight
#if defined(PCBX9E)
  #define BACKLIGHT_RCC_AHB1Periph      RCC_AHB1Periph_GPIOE
  #define BACKLIGHT_RCC_APB1Periph      0
  #define BACKLIGHT_RCC_APB2Periph      RCC_APB2Periph_TIM9
  #define BACKLIGHT_TIMER_FREQ          (PERI2_FREQUENCY * TIMER_MULT_APB2)
  #define BACKLIGHT_TIMER               TIM9
  #define BACKLIGHT_GPIO                GPIOE
  #define BACKLIGHT_GPIO_PIN_1          GPIO_Pin_6 // PE.06
  #define BACKLIGHT_GPIO_PIN_2          GPIO_Pin_5 // PE.05
  #define BACKLIGHT_GPIO_PinSource_1    GPIO_PinSource6
  #define BACKLIGHT_GPIO_PinSource_2    GPIO_PinSource5
  #define BACKLIGHT_GPIO_AF_1           GPIO_AF_TIM9
  #define BACKLIGHT_GPIO_AF_2           GPIO_AF_TIM9
#elif defined (PCBX9DP)
  #define BACKLIGHT_RCC_AHB1Periph      RCC_AHB1Periph_GPIOD
  #define BACKLIGHT_RCC_APB1Periph      RCC_APB1Periph_TIM4
  #define BACKLIGHT_RCC_APB2Periph      0
  #define BACKLIGHT_TIMER_FREQ          (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define BACKLIGHT_TIMER               TIM4
  #define BACKLIGHT_GPIO                GPIOD
  #define BACKLIGHT_GPIO_PIN_1          GPIO_Pin_15 // PD.15
  #define BACKLIGHT_GPIO_PIN_2          GPIO_Pin_13 // PD.13
  #define BACKLIGHT_GPIO_PinSource_1    GPIO_PinSource15
  #define BACKLIGHT_GPIO_PinSource_2    GPIO_PinSource13
  #define BACKLIGHT_GPIO_AF_1           GPIO_AF_TIM4
  #define BACKLIGHT_GPIO_AF_2           GPIO_AF_TIM4
#elif defined(PCBX7)
  #define BACKLIGHT_RCC_AHB1Periph      RCC_AHB1Periph_GPIOD
  #define BACKLIGHT_RCC_APB1Periph      RCC_APB1Periph_TIM4
  #define BACKLIGHT_RCC_APB2Periph      0
  #define BACKLIGHT_TIMER_FREQ          (PERI1_FREQUENCY * TIMER_MULT_APB1)
  #define BACKLIGHT_TIMER               TIM4
  #define BACKLIGHT_GPIO                GPIOD
  #define BACKLIGHT_GPIO_PIN            GPIO_Pin_13 // PD.13
  #define BACKLIGHT_GPIO_PinSource      GPIO_PinSource13
  #define BACKLIGHT_GPIO_AF             GPIO_AF_TIM4
#else
  #define BACKLIGHT_RCC_AHB1Periph      RCC_AHB1Periph_GPIOB
  #define BACKLIGHT_RCC_APB1Periph      0
  #define BACKLIGHT_RCC_APB2Periph      RCC_APB2Periph_TIM10
  #define BACKLIGHT_TIMER_FREQ          (PERI2_FREQUENCY * TIMER_MULT_APB2)
  #define BACKLIGHT_TIMER               TIM10
  #define BACKLIGHT_GPIO                GPIOB
  #define BACKLIGHT_GPIO_PIN            GPIO_Pin_8  // PB.08
  #define BACKLIGHT_GPIO_PinSource      GPIO_PinSource8
  #define BACKLIGHT_GPIO_AF             GPIO_AF_TIM10
#endif

// LCD driver
#if defined(PCBX9E)
  #define LCD_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
  #define LCD_RCC_APB1Periph            RCC_APB1Periph_SPI3
  #define LCD_SPI_GPIO                  GPIOC
  #define LCD_MOSI_GPIO_PIN             GPIO_Pin_12 // PC.12
  #define LCD_MOSI_GPIO_PinSource       GPIO_PinSource12
  #define LCD_CLK_GPIO_PIN              GPIO_Pin_10 // PC.10
  #define LCD_CLK_GPIO_PinSource        GPIO_PinSource10
  #define LCD_A0_GPIO_PIN               GPIO_Pin_11 // PC.11
  #define LCD_NCS_GPIO                  GPIOA
  #define LCD_NCS_GPIO_PIN              GPIO_Pin_15 // PA.15
  #define LCD_RST_GPIO                  GPIOD
  #define LCD_RST_GPIO_PIN              GPIO_Pin_15 // PD.15
  #define LCD_DMA                       DMA1
  #define LDC_DMA_Stream                DMA1_Stream7
  #define LCD_DMA_Stream_IRQn           DMA1_Stream7_IRQn
  #define LCD_DMA_Stream_IRQHandler     DMA1_Stream7_IRQHandler
  #define LCD_DMA_FLAGS                 (DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7)
  #define LCD_DMA_FLAG_INT              DMA_HIFCR_CTCIF7
  #define LCD_SPI                       SPI3
  #define LCD_GPIO_AF                   GPIO_AF_SPI3
#elif defined(PCBX9DP) || defined(PCBX7)
  #define LCD_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1)
  #define LCD_RCC_APB1Periph            RCC_APB1Periph_SPI3
  #define LCD_SPI_GPIO                  GPIOC
  #define LCD_MOSI_GPIO_PIN             GPIO_Pin_12 // PC.12
  #define LCD_MOSI_GPIO_PinSource       GPIO_PinSource12
  #define LCD_CLK_GPIO_PIN              GPIO_Pin_10 // PC.10
  #define LCD_CLK_GPIO_PinSource        GPIO_PinSource10
  #define LCD_A0_GPIO_PIN               GPIO_Pin_11 // PC.11
  #define LCD_NCS_GPIO                  GPIOA
  #define LCD_NCS_GPIO_PIN              GPIO_Pin_15 // PA.15
  #define LCD_RST_GPIO                  GPIOD
  #define LCD_RST_GPIO_PIN              GPIO_Pin_12 // PD.12
  #define LCD_DMA                       DMA1
  #define LDC_DMA_Stream                DMA1_Stream7
  #define LCD_DMA_Stream_IRQn           DMA1_Stream7_IRQn
  #define LCD_DMA_Stream_IRQHandler     DMA1_Stream7_IRQHandler
  #define LCD_DMA_FLAGS                 (DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7)
  #define LCD_DMA_FLAG_INT              DMA_HIFCR_CTCIF7
  #define LCD_SPI                       SPI3
  #define LCD_GPIO_AF                   GPIO_AF_SPI3
#else
  #define LCD_RCC_AHB1Periph            RCC_AHB1Periph_GPIOD
  #define LCD_RCC_APB1Periph            0
  #define LCD_SPI_GPIO                  GPIOD
  #define LCD_MOSI_GPIO_PIN             GPIO_Pin_10 // PD.10
  #define LCD_CLK_GPIO_PIN              GPIO_Pin_11 // PD.11
  #define LCD_A0_GPIO_PIN               GPIO_Pin_13 // PD.13
  #define LCD_NCS_GPIO                  GPIOD
  #define LCD_NCS_GPIO_PIN              GPIO_Pin_14 // PD.14
  #define LCD_RST_GPIO                  GPIOD
  #define LCD_RST_GPIO_PIN              GPIO_Pin_12 // PD.12
#endif

// I2C Bus: EEPROM and CAT5137 digital pot for volume control
#define I2C_RCC_AHB1Periph              RCC_AHB1Periph_GPIOB
#define I2C_RCC_APB1Periph              RCC_APB1Periph_I2C1
#define I2C                             I2C1
#define I2C_GPIO                        GPIOB
#define I2C_SCL_GPIO_PIN                GPIO_Pin_6  // PB.06
#define I2C_SDA_GPIO_PIN                GPIO_Pin_7  // PB.07
#define I2C_GPIO_PIN_WP                 GPIO_Pin_9  // PB.09
#define I2C_GPIO_AF                     GPIO_AF_I2C1
#define I2C_SCL_GPIO_PinSource          GPIO_PinSource6
#define I2C_SDA_GPIO_PinSource          GPIO_PinSource7
#if defined(REV4a)
  #define I2C_SPEED                     100000
#else
  #define I2C_SPEED                     400000
#endif
#define I2C_ADDRESS_EEPROM              0xA2
#define I2C_ADDRESS_VOLUME              0x5C
#define I2C_FLASH_PAGESIZE              64

// SD - SPI2
#define SD_RCC_AHB1Periph               (RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA1)
#define SD_RCC_APB1Periph               RCC_APB1Periph_SPI2
#define SD_GPIO_PRESENT                 GPIOD
#define SD_GPIO_PIN_PRESENT             GPIO_Pin_9  // PD.09
#define SD_GPIO                         GPIOB
#define SD_GPIO_PIN_CS                  GPIO_Pin_12 // PB.12
#define SD_GPIO_PIN_SCK                 GPIO_Pin_13 // PB.13
#define SD_GPIO_PIN_MISO                GPIO_Pin_14 // PB.14
#define SD_GPIO_PIN_MOSI                GPIO_Pin_15 // PB.15
#define SD_GPIO_AF                      GPIO_AF_SPI2
#define SD_GPIO_PinSource_CS            GPIO_PinSource12
#define SD_GPIO_PinSource_SCK           GPIO_PinSource13
#define SD_GPIO_PinSource_MISO          GPIO_PinSource14
#define SD_GPIO_PinSource_MOSI          GPIO_PinSource15
#define SD_SPI                          SPI2
#define SD_SPI_BaudRatePrescaler        SPI_BaudRatePrescaler_4 // 10.5<20MHZ, make sure < 20MHZ

#if !defined(BOOT)
  #define SD_USE_DMA                    // Enable the DMA for SD
  #define SD_DMA_Stream_SPI_RX          DMA1_Stream3
  #define SD_DMA_Stream_SPI_TX          DMA1_Stream4
  #define SD_DMA_FLAG_SPI_TC_RX         DMA_FLAG_TCIF3
  #define SD_DMA_FLAG_SPI_TC_TX         DMA_FLAG_TCIF4
  #define SD_DMA_Channel_SPI            DMA_Channel_0
#endif

// Audio
#define AUDIO_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_DMA1)
#define AUDIO_RCC_APB1Periph            (RCC_APB1Periph_TIM6 | RCC_APB1Periph_DAC)
#define AUDIO_OUTPUT_GPIO               GPIOA
#define AUDIO_OUTPUT_GPIO_PIN           GPIO_Pin_4  // PA.04
#define AUDIO_DMA_Stream                DMA1_Stream5
#define AUDIO_DMA_Stream_IRQn           DMA1_Stream5_IRQn
#define AUDIO_TIM_IRQn                  TIM6_DAC_IRQn
#define AUDIO_TIM_IRQHandler            TIM6_DAC_IRQHandler
#define AUDIO_DMA_Stream_IRQHandler     DMA1_Stream5_IRQHandler
#define AUDIO_TIMER                     TIM6
#define AUDIO_DMA                       DMA1

// Haptic
#if defined(PCBX9E) || defined(PCBX9DP) || defined(PCBX7)
  #define HAPTIC_RCC_AHB1Periph         RCC_AHB1Periph_GPIOB
  #define HAPTIC_RCC_APB2Periph         RCC_APB2Periph_TIM10
  #define HAPTIC_GPIO_PinSource         GPIO_PinSource8
  #define HAPTIC_GPIO                   GPIOB
  #define HAPTIC_GPIO_PIN               GPIO_Pin_8  // PB.08
  #define HAPTIC_GPIO_AF                GPIO_AF_TIM10
  #define HAPTIC_TIMER                  TIM10
#else
  #define HAPTIC_RCC_AHB1Periph         RCC_AHB1Periph_GPIOC
  #define HAPTIC_RCC_APB2Periph         0
  #define HAPTIC_GPIO                   GPIOC
  #define HAPTIC_GPIO_PIN               GPIO_Pin_12
#endif

// Top LCD on X9E
#if defined(PCBX9E)
  #define TOPLCD_RCC_AHB1Periph         RCC_AHB1Periph_GPIOG
  #define TOPLCD_GPIO                   GPIOG
  #define TOPLCD_GPIO_PIN_DATA          GPIO_Pin_5
  #define TOPLCD_GPIO_PIN_WR            GPIO_Pin_4
  #define TOPLCD_GPIO_PIN_CS1           GPIO_Pin_3
  #define TOPLCD_GPIO_PIN_CS2           GPIO_Pin_15
  #define TOPLCD_GPIO_PIN_BL            GPIO_Pin_2
#endif

// Bluetooth
#if defined(PCBX9E)
  #define BT_USART                     USART6
  #define BT_GPIO_AF                   GPIO_AF_USART6
  #define BT_USART_IRQn                USART6_IRQn
  #define BT_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB)
  #define BT_GPIO_TXRX                 GPIOG
  #define BT_TX_GPIO_PIN               GPIO_Pin_14 // PG.14
  #define BT_RX_GPIO_PIN               GPIO_Pin_9  // PG.09
  #define BT_EN_GPIO                   GPIOD
  #define BT_EN_GPIO_PIN               GPIO_Pin_11 // PD.11
  #define BT_BRTS_GPIO                 GPIOE
  #define BT_BRTS_GPIO_PIN             GPIO_Pin_12 // PE.12
  #define BT_BCTS_GPIO                 GPIOG
  #define BT_BCTS_GPIO_PIN             GPIO_Pin_6  // PG.06
  #define BT_TX_GPIO_PinSource         GPIO_PinSource14
  #define BT_RX_GPIO_PinSource         GPIO_PinSource9
#elif defined(PCBX7)
  #define BT_RCC_AHB1Periph            (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_DMA1)
  #define BT_RCC_APB1Periph            RCC_APB1Periph_USART3
  #define BT_EN_GPIO                   GPIOE
  #define BT_EN_GPIO_PIN               GPIO_Pin_12 // PE.12
  #define BT_GPIO_TXRX                 GPIOB
  #define BT_TX_GPIO_PIN               GPIO_Pin_10 // PB.10
  #define BT_RX_GPIO_PIN               GPIO_Pin_11 // PB.11
  #define BT_TX_GPIO_PinSource         GPIO_PinSource10
  #define BT_RX_GPIO_PinSource         GPIO_PinSource11
  #define BT_GPIO_AF                   GPIO_AF_USART3
  #define BT_USART                     USART3
  #define BT_USART_IRQHandler          USART3_IRQHandler
  #define BT_USART_IRQn                USART3_IRQn
  #define BT_DMA_Stream_RX             DMA1_Stream1
  #define BT_DMA_Channel_RX            DMA_Channel_4
#endif

// 5ms Interrupt
#define INTERRUPT_5MS_APB1Periph        RCC_APB1Periph_TIM14
#define INTERRUPT_5MS_TIMER             TIM14
#define INTERRUPT_5MS_IRQn              TIM8_TRG_COM_TIM14_IRQn
#define INTERRUPT_5MS_IRQHandler        TIM8_TRG_COM_TIM14_IRQHandler

// 2MHz Timer
#define TIMER_2MHz_APB1Periph           RCC_APB1Periph_TIM7
#define TIMER_2MHz_TIMER                TIM7

#endif // _HAL_H_
