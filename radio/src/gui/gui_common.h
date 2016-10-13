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

#ifndef _GUI_COMMON_H_
#define _GUI_COMMON_H_

// model_setup Defines that are used in all uis in the same way
#define EXTERNAL_MODULE_CHANNELS_ROWS   IF_EXTERNAL_MODULE_ON((IS_MODULE_DSM2(EXTERNAL_MODULE) || IS_MODULE_CROSSFIRE(EXTERNAL_MODULE) || (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) != MM_RF_PROTO_DSM2)) ? (uint8_t)0 : (uint8_t)1)


#if defined(MULTIMODULE)
#define MULTIMODULE_MODULE_ROWS         IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW, IS_MODULE_MULTIMODULE(EXTERNAL_MODULE) ? (uint8_t) 0 : HIDDEN_ROW,
#define MULTIMODULE_MODE_ROWS(x)        (g_model.moduleData[x].multi.customProto) ? (uint8_t) 3 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol(true)) ? (uint8_t)2 : (uint8_t)1
#define MULTIMODULE_RFPROTO_ROWS(x)     (g_model.moduleData[x].multi.customProto) ? (uint8_t) 1 :MULTIMODULE_HAS_SUBTYPE(g_model.moduleData[x].getMultiProtocol(true)) ? (uint8_t) 0 : HIDDEN_ROW
#define MULTIMODULE_SUBTYPE_ROWS(x)     IS_MODULE_MULTIMODULE(x) ? MULTIMODULE_RFPROTO_ROWS(x) : HIDDEN_ROW,
#define MULTIMODULE_HAS_SUBTYPE(x)      (x == MM_RF_PROTO_FLYSKY || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_HISKY || x == MM_RF_PROTO_DSM2 || x == MM_RF_PROTO_YD717 || x == MM_RF_PROTO_KN || x == MM_RF_PROTO_SYMAX || x == MM_RF_PROTO_CX10 || x == MM_RF_PROTO_CG023 || x == MM_RF_PROTO_MT99XX || x == MM_RF_PROTO_MJXQ || x == MM_RF_PROTO_HONTAI || x == MM_RF_CUSTOM_SELECTED)
#define MULTIMODULE_HASOPTIONS(x)       (x == MM_RF_PROTO_HUBSAN || x == MM_RF_PROTO_FRSKY || x == MM_RF_PROTO_DSM2 || x == MM_RF_PROTO_SFHSS || x == MM_RF_CUSTOM_SELECTED)
#define MULTIMODULE_FAILSAFEROWS(x)     (IS_MODULE_MULTIMODULE(x) && (MULTIMODULE_HASOPTIONS(g_model.moduleData[x].getMultiProtocol(true)))) ? (uint8_t) 0: HIDDEN_ROW

#else
#define MULTIMODULE_MODULE_ROWS
#define MULTIMODULE_FAILSAFEROWS(x)     HIDDEN_ROW
#define MULTIMODULE_SUBTYPE_ROWS(x)
#define MULTIMODULE_MODE_ROWS(x)        (uint8_t)0
#endif

#define IS_D8_RX(x)                    (g_model.moduleData[x].rfProtocol == RF_PROTO_D8)
#define IF_TRAINER_ON(x)               (g_model.trainerMode == TRAINER_MODE_SLAVE ? (uint8_t)(x) : HIDDEN_ROW)

#define FAILSAFE_ROWS(x)               (IS_MODULE_XJT(x) && HAS_RF_PROTOCOL_FAILSAFE(g_model.moduleData[x].rfProtocol) ? (g_model.moduleData[x].failsafeMode==FAILSAFE_CUSTOM ? (uint8_t)1 : (uint8_t)0) : MULTIMODULE_FAILSAFEROWS(x))

#endif // _GUI_COMMON_H_
