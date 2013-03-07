/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

// NON ZERO TERMINATED STRINGS
#define LEN_OFFON        "\003"
#define TR_OFFON         "OFF""ON\0"

#define LEN_MMMINV       "\003"
#define TR_MMMINV        "---""INV"

#define LEN_NCHANNELS    "\004"
#define TR_NCHANNELS     "\0014CH\0016CH\0018CH10CH12CH14CH16CH"

#define LEN_VBEEPMODE    "\005"
#define TR_VBEEPMODE     "Mudo ""Alarm""NoKey""Todo "

#define LEN_VBEEPLEN     "\005"
#define TR_VBEEPLEN      "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG     "\003"
#define TR_VRENAVIG      "No REaREb"

#define LEN_VBLMODE      "\004"
#define TR_VBLMODE       "OFF ""Keys""Stks""Both""ON\0"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "OFF"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "CH1CH2CH3CH4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATA:""HORA:"

#define LEN_VLCD         "\006"
#define TR_VLCD          "NormalOptrex"

#define LEN_VTRIMINC     TR("\006","\013")
#define TR_VTRIMINC      TR("Expo  ""ExFino""Fino  ""Medio ""Grueso","Exponencial""Extra Fino ""Fino       ""Medio      ""Grueso     ")

#define LEN_RETA123      "\001"

#if defined(PCBGRUVIN9X)
  #if ROTARY_ENCODERS > 2
    #define TR_RETA123       "RETA123abcd"
  #else
    #define TR_RETA123       "RETA123ab"
  #endif
#elif defined(PCBX9D)
  #define TR_RETA123         "RETA12LR"
#else
  #define TR_RETA123         "RETA123"
#endif

#define LEN_VPROTOS      "\006"

#if defined(PXX)
  #define TR_PXX         "PXX\0  "
#elif defined(DSM2) || defined(IRPROTOS)
  #define TR_PXX         "[PXX]\0"
#else
  #define TR_PXX
#endif

#if defined(DSM2)
  #define TR_DSM2        "LP45\0 ""DSM2\0 ""DSMX\0 "
#elif defined(IRPROTOS)
  #define TR_DSM2        "[LP45]""[DSM2]""[DSMX]"
#else
  #define TR_DSM2
#endif

#if defined(IRPROTOS)
  #define TR_IRPROTOS    "SILV  TRAC09PICZ  SWIFT\0"
#else
  #define TR_IRPROTOS
#endif

#if defined(CPUARM)
  #define TR_XPPM
#else
  #define TR_XPPM              "PPM16\0""PPMsim"
#endif

#define TR_VPROTOS             "PPM\0  " TR_XPPM TR_PXX TR_DSM2 TR_IRPROTOS

#define LEN_POSNEG       "\003"
#define TR_POSNEG        "POS""NEG"

#define LEN_VCURVEFUNC   "\003"
#define TR_VCURVEFUNC    "---""x>0""x<0""|x|""f>0""f<0""|f|"

#define LEN_VMLTPX       "\010"
#define TR_VMLTPX        "Agregar ""Multipl.""Cambiar "

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\003"
#define TR_VMIXTRIMS     "OFF""ON\0""Dir""Ele""Ace""Ale"

#define LEN_VCSWFUNC     "\006"
#define TR_VCSWFUNC      "---\0  ""v1==x\0""v1>x\0 ""v1<x\0 ""|v1|>x""|v1|<x""AND\0  ""OR\0   ""XOR\0  ""v1==v2""v1>v2\0""v1<v2\0""d>=x\0 ""|d|>=x"

#define LEN_VFSWFUNC     "\013"

#if defined(VARIO)
  #define TR_VVARIO        "Vario\0     "
#else
  #define TR_VVARIO        "[Vario]\0   "
#endif

#if defined(AUDIO)
  #define TR_SOUND         "Oir sonido\0"
#else
  #define TR_SOUND         "Beep\0      "
#endif

#if defined(HAPTIC)
  #define TR_HAPTIC        "Tactil\0    "
#else
  #define TR_HAPTIC        "[Tactil]\0  "
#endif

#if defined(VOICE)
  #if defined(PCBSKY9X)
    #define TR_PLAY_TRACK    "Oir pista \0"
  #else
    #define TR_PLAY_TRACK    "Oir\0       "
  #endif
  #define TR_PLAY_BOTH       "Play Both\0"
  #define TR_PLAY_VALUE      "Oir valor \0"
#else
  #define TR_PLAY_TRACK    "[Oir pista]"
  #define TR_PLAY_BOTH     "[Play Both]"
  #define TR_PLAY_VALUE    "[Oir Valor]"
#endif

#define TR_CFN_VOLUME    "Volumen\0   "
#define TR_CFN_BG_MUSIC  "BgMusica\0  ""BgMusica||\0"

#if defined(SDCARD)
  #define TR_SDCLOGS     "SD Logs\0   "
#else
  #define TR_SDCLOGS     "[SD Logs]\0 "
#endif

#ifdef GVARS
  #define TR_CFN_ADJUST_GVAR  "Ajuste \0   "
#else
  #define TR_CFN_ADJUST_GVAR
#endif

#ifdef DEBUG
  #define TR_CFN_TEST      "Test\0"
#else
  #define TR_CFN_TEST
#endif

#if defined(CPUARM)
  #define TR_VFSWFUNC      "Seguro\0    ""Aprendiz\0  ""Inst. Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_CFN_VOLUME "Panel luz\0 " TR_CFN_BG_MUSIC TR_CFN_ADJUST_GVAR TR_CFN_TEST
#elif defined(PCBGRUVIN9X)
  #define TR_VFSWFUNC      "Seguro\0    ""Aprendiz\0  ""Inst. Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE TR_SDCLOGS "Panel luz\0 " TR_CFN_ADJUST_GVAR TR_CFN_TEST
#else
  #define TR_VFSWFUNC      "Seguro\0    ""Aprendiz\0  ""Inst. Trim\0" TR_SOUND TR_HAPTIC "Reset\0     " TR_VVARIO TR_PLAY_TRACK TR_PLAY_BOTH TR_PLAY_VALUE "Panel luz\0 " TR_CFN_ADJUST_GVAR TR_CFN_TEST
#endif

#define LEN_VFSWRESET      "\006"

#if defined(FRSKY)
  #define TR_FSW_RESET_TELEM     "Telem."
#else
  #define TR_FSW_RESET_TELEM   
#endif

#if ROTARY_ENCODERS > 0
  #define TR_FSW_RESET_ROTENC  "R.Enc"
#else
  #define TR_FSW_RESET_ROTENC
#endif

#define TR_VFSWRESET     "Reloj1""Reloj2""Todo  " TR_FSW_RESET_TELEM TR_FSW_RESET_ROTENC

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Beep1 ""Beep2 ""Beep3 ""Aviso1""Aviso2""Cheep ""Ring  ""SciFi ""Robot ""Gorjeo""Tada  ""Crickt""Sirena""Alarma""Ratata""Tictac"

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Batt""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Cur+""Acc\0""Hora"

#ifdef IMPERIAL_UNITS
  #define LENGTH_UNIT "ft\0"
  #define SPEED_UNIT  "kts"
#else
  #define LENGTH_UNIT "m\0 "
  #define SPEED_UNIT  "kmh"
#endif

#define LEN_VTELEMUNIT   "\003"
#define TR_VTELEMUNIT    "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#define STR_V            (STR_VTELEMUNIT+1)
#define STR_A            (STR_VTELEMUNIT+4)

#define LEN_VALARM       "\003"
#define TR_VALARM        "---""Ama""Nar""Roj"

#define LEN_VALARMFN     "\001"
#define TR_VALARMFN      "<>"

#define LEN_VTELPROTO    "\007"
#define TR_VTELPROTO     "Nada\0  ""Hub\0   ""WSHHigh"

#define LEN_VOLTSRC      "\003"
#define TR_VOLTSRC       "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC     "\005"
#define TR_VARIOSRC      "Alti\0""Alti+""Vario""A1\0  ""A2\0"

#define LEN_VSCREEN      "\004"
#define TR_VSCREEN       "Nums""Bars"

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Elim Mezcla\0\0""Simple 4-CH \0""Anular Motor\0""Cola en V   \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu""Salir""Abajo""Arrib""Dcha.""Izqd."

#define LEN_VRENCODERS   "\003"
#define TR_VRENCODERS    "REa""REb"

#define LEN_VSWITCHES    "\003"
#define LEN_VSRCRAW      "\004"

#if defined(PCBX9D)
  #define TR_POTS_VSRCRAW      "S1\0 ""S2\0 ""LS\0 ""RS\0 "
  #define TR_SW_VSRCRAW        "SA\0 ""SB\0 ""SC\0 ""SD\0 ""SE\0 ""SF\0 ""SG\0 ""SH\0 "
#elif defined(EXTRA_3POS)
  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 "
  #define TR_SW_VSRCRAW        "3P1\0""3P2\0"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2""ID3""ID4""ID5"
#else
  #define TR_POTS_VSRCRAW      "P1\0 ""P2\0 ""P3\0 "
  #define TR_SW_VSRCRAW        "3POS"
  #define TR_9X_3POS_SWITCHES  "ID0""ID1""ID2"
#endif

#if defined(CPUARM)
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF""CSG""CSH""CSI""CSJ""CSK""CSL""CSM""CSN""CSO""CSP""CSQ""CSR""CSS""CST""CSU""CSV""CSW"
#elif defined(PCBGRUVIN9X) || defined(CPUM128)
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF"
#else
  #define TR_CUSTOMSW          "CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC"
#endif

#if defined(PCBX9D)
  #define TR_VSWITCHES         "SA\300""SA-""SA\301""SB\300""SB-""SB\301""SC\300""SC-""SC\301""SD\300""SD-""SD\301""SE\300""SE-""SE\301""SF\300""SF\301""SG\300""SG-""SG\301""SH\300""SH\301" TR_CUSTOMSW " ON"
#else
  #define TR_VSWITCHES         TR_9X_3POS_SWITCHES "THR""RUD""ELE""AIL""GEA""TRN" TR_CUSTOMSW " ON"
#endif

#define LEN_VSWITCHES_SHORT    "\001"
#define TR_VSWITCHES_SHORT     "-012TREAG3456789"

#if defined(PCBSKY9X)
  #define TR_ROTARY_ENCODERS_VSRCRAW "REnc"
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS > 2
  #define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS <= 2
  #define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#else
  #define TR_ROTARY_ENCODERS_VSRCRAW
#endif

#if defined(HELI)
  #define TR_CYC_VSRCRAW   "CYC1""CYC2""CYC3"
#else
  #define TR_CYC_VSRCRAW   "[C1]""[C2]""[C3]"
#endif

#define TR_VSRCRAW       "---\0""Dir\0""Ele\0""Ace\0""Ale\0" TR_POTS_VSRCRAW TR_ROTARY_ENCODERS_VSRCRAW "MAX " TR_CYC_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" TR_SW_VSRCRAW 

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#if defined(PCBX9D)
#define TR_POPUPS              "[INTRO]\010[SALIR]"
#else
#define TR_POPUPS              "[MENU]\010[SALIR]"
#endif
#define OFS_EXIT               7
#if defined(PCBX9D)
  #define TR_MENUWHENDONE      CENTER"\006[INTRO]AL ACABAR"
#else
  #define TR_MENUWHENDONE      CENTER"\006[MENU]AL ACABAR"
#endif
#define TR_FREE                "libre"
#define TR_DELETEMODEL         "BORRAR MODELO"
#define TR_COPYINGMODEL        "Copiando modelo.."
#define TR_MOVINGMODEL         "Mover modelo..."
#define TR_LOADINGMODEL        "Cargar modelo..."
#define TR_NAME                "Nombre"
#define TR_MODELNAME           "Nom.Modelo"
#define TR_PHASENAME           "Nom. Fase"
#define TR_MIXNAME             "Nom.Mezcla"
#define TR_EXPONAME            "Nom. Expo"
#define TR_BITMAP              "Imagen Modelo"
#define TR_TIMER               TR("Reloj","Reloj ")
#define TR_ELIMITS             TR("E.Limite","Ampliar Limites")
#define TR_ETRIMS              TR("E.Trims","Ampliar Trims")
#define TR_TRIMINC             "Trim Step"
#define TR_TTRACE              TR("T-Source","Fuente Acelerad.")
#define TR_TTRIM               TR("T-Trim","Trim Acelerad.")
#define TR_BEEPCTR             TR("Ctr Beep","Center Beep")
#define TR_PROTO               TR(INDENT"Proto",INDENT"Protocolo")
#define TR_PPMFRAME            "Trama PPM"
#define TR_MS                  "ms"
#define TR_SWITCH              "Interr"
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Inicio"
#define TR_FADEOUT             "Final"
#define TR_DEFAULT             "(defecto)"
#define TR_CHECKTRIMS          CENTER"\006Check\012Trims"
#define OFS_CHECKTRIMS         CENTER_OFS+(9*FW)
#define TR_SWASHTYPE           "Tipo Ciclico"
#define TR_COLLECTIVE          "Colectivo"
#define TR_SWASHRING           "Ciclico"
#define TR_ELEDIRECTION        "ELE Direccion"
#define TR_AILDIRECTION        "AIL Direccion"
#define TR_COLDIRECTION        "COL Direccion"
#define TR_MODE                INDENT"Modo"
#define TR_NOFREEEXPO          "No free expo!"
#define TR_NOFREEMIXER         "No free mixer!"
#define TR_INSERTMIX           "INSERTAR MIX"
#define TR_EDITMIX             "EDITAR MIX"
#define TR_SOURCE              INDENT"Fuente"
#define TR_WEIGHT              "Cantidad"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Lado"
#define TR_DIFFERENTIAL        "Diferenc"
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "Curva"
#define TR_FPHASE              "Fase"
#define TR_MIXWARNING          "Aviso"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "Multpx"
#define TR_DELAYDOWN           "Delay Dn"
#define TR_DELAYUP             "Delay Up"
#define TR_SLOWDOWN            "Slow  Dn"
#define TR_SLOWUP              "Slow  Up"
#define TR_MIXER               "Mezclas"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004canal"
#define TR_RANGE               INDENT"Alcance"
#define TR_BAR                 "Bar"
#define TR_ALARM               INDENT"Alarma"
#define TR_USRDATA             "UsrData"
#define TR_BLADES              INDENT"Palas"
#define TR_SCREEN              "Pantalla"
#define TR_SOUND_LABEL         "Sonido"
#define TR_LENGTH              INDENT"Longitud"
#define TR_SPKRPITCH           INDENT"Tono"
#define TR_HAPTIC_LABEL        "Tactil"
#define TR_HAPTICSTRENGTH      INDENT"Intensidad"
#define TR_CONTRAST            "Contraste"
#define TR_ALARMS_LABEL        "Alarmas"
#define TR_BATTERYWARNING      INDENT"Bateria Baja"
#define TR_INACTIVITYALARM     INDENT"Inactividad"
#define TR_MEMORYWARNING       INDENT"Memoria Baja"
#define TR_ALARMWARNING        INDENT"Sin Sonido"
#define TR_RENAVIG             "RotEnc Naveg"
#define TR_THROTTLEREVERSE     TR("Acel Invert","Invertir Acel.")
#define TR_MINUTEBEEP          TR(INDENT"Minuto",INDENT"Cada minuto")
#define TR_BEEPCOUNTDOWN       INDENT"Cuentaatras"
#define TR_BACKLIGHT_LABEL     "Luz Fondo"
#define TR_BLDELAY             INDENT"Duracion"
#define TR_BLONBRIGHTNESS      INDENT"ON Brightness"
#define TR_BLOFFBRIGHTNESS     INDENT"OFF Brightness"
#define TR_SPLASHSCREEN        "Ptalla. Inicio"
#define TR_THROTTLEWARNING     TR("A-Aviso","Aviso Aceler.")
#define TR_SWITCHWARNING       TR("I-Aviso","Aviso Intrrptor")
#define TR_TIMEZONE            TR("Zona Hora","GPS Zona Horar.")
#define TR_RXCHANNELORD        TR("Rx Channel Ord","Orden habitual canales")
#define TR_SLAVE               "Esclavo"
#define TR_MODESRC             "Modo\006% Fuente"
#define TR_MULTIPLIER          "Multiplicar"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"

#if defined(PCBX9D)
  #define TR_MENUTOSTART       CENTER"\006[INTRO] EMPEZAR"
#else
  #define TR_MENUTOSTART       CENTER"\006[MENU] EMPEZAR"
#endif

#define TR_SETMIDPOINT         CENTER"\003STICKS AL CENTRO"
#define TR_MOVESTICKSPOTS      CENTER"\006MOVER STICKS/POTS"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              "SIN DATOS"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"

#if defined(CPUARM)
  #define TR_TMIXMAXMS         "Tmix max\012ms"
#else
  #define TR_TMIXMAXMS         "Tmix max\014ms"
#endif

#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Pila libre\010b"
#define TR_MENUTORESET         "[MENU] Resetea"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODELO"
#define TR_FP                  "FP"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM Mem. baja"
#define TR_ALERT               "\016ALERTA"
#define TR_PRESSANYKEYTOSKIP   CENTER"Pulsar tecla omitir"
#define TR_THROTTLENOTIDLE     CENTER"Aceler. Activado "
#define TR_ALARMSDISABLED      CENTER"Alarmas Desact."
#define TR_PRESSANYKEY         TR("\010Pulsa una tecla",CENTER"Pulsa una tecla")
#define TR_BADEEPROMDATA       CENTER"Datos EEprom mal"
#define TR_EEPROMFORMATTING    CENTER"Formateando EEPROM"
#define TR_EEPROMOVERFLOW      CENTER"Desborde EEPROM"
#define TR_MENURADIOSETUP      "CONFIGURACION"
#define TR_MENUDATEANDTIME     "FECHA Y HORA"
#define TR_MENUTRAINER         "APRENDIZ"
#define TR_MENUVERSION         "VERSION"
#define TR_MENUDIAG            "DIAG"
#define TR_MENUANA             "ANAS"
#define TR_MENUCALIBRATION     "CALIBRACION"
#define TR_TRIMS2OFFSETS       "\006Trims => Offsets"
#define TR_MENUMODELSEL        TR("MODELSEL","SELECION MODELO")
#define TR_MENUSETUP           TR("CONF.","CONF. MODELO")
#define TR_MENUFLIGHTPHASE     "FASE VUELO"
#define TR_MENUFLIGHTPHASES    "FASES VUELO"
#define TR_MENUHELISETUP       "CONF.HELI"

#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // Los titulos del menu derecho para gurus ...
  #define TR_MENUDREXPO        "STICKS"
  #define TR_MENULIMITS        "SALIDAS"
#else
  #define TR_MENUDREXPO        "DR/EXPO"
  #define TR_MENULIMITS        "LIMITES"
#endif

#define TR_MENUCURVES          "CURVAS"
#define TR_MENUCURVE           "CURVA"
#define TR_MENUCUSTOMSWITCH    "AJTE.INTERUP."
#define TR_MENUCUSTOMSWITCHES  "AJTE.INTERUPTS"
#define TR_MENUCUSTOMFUNC      "AJTE.FUNCIONES"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "PLANTILLAS"
#define TR_MENUSTAT            "STATS"
#define TR_MENUDEBUG           "DEBUG"
#define TR_RXNUM               TR("RxNum","Numero Receptor")
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_LIMIT               INDENT"Limite"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitud"
#define TR_LONGITUDE           "Longitud"
#define TR_GPSCOORD            TR("GPS Coords","GPS formatear coords.")
#define TR_VARIO               TR("Vario","Variometro")
#define TR_SHUTDOWN            "APAGANDO"
#define TR_BATT_CALIB          "Calib.bateria"
#define TR_CURRENT_CALIB       "Calib.Actual"
#define TR_VOLTAGE             INDENT"Voltaje"
#define TR_CURRENT             INDENT"Actual"
#define TR_SELECT_MODEL        "Selec Modelo"
#define TR_CREATE_MODEL        "Crear Modelo"
#define TR_COPY_MODEL          "Copy Model"
#define TR_MOVE_MODEL          "Move Model"
#define TR_BACKUP_MODEL        "Backup Modelo"
#define TR_DELETE_MODEL        "Borrar Modelo"
#define TR_RESTORE_MODEL       "Recuper Modelo"
#define TR_SDCARD_ERROR        "SDCARD Error"
#define TR_NO_SDCARD           "No SDCARD"
#define TR_INCOMPATIBLE        "Incompatible"
#define TR_WARNING             "AVISO"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "ACELERADOR"
#define TR_ALARMSWARN          "ALARMAS"
#define TR_SWITCHWARN          "INTERPTOR"
#define TR_INVERT_THR          TR("Invertir Acel?","Invertir Acel.?")
#define TR_SPEAKER_VOLUME      INDENT"Volumen"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Brillo"
#define TR_CPU_TEMP            "CPU Temp.\016>"
#define TR_CPU_CURRENT         "Actual\022>"
#define TR_CPU_MAH             "Consumpt."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT"Capacidad Baja"
#define TR_TEMPWARNING         TR(INDENT"Temp. Alta",INDENT"Temperatura Alta")
#define TR_FUNC                "Func"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duracion"
#define TR_DELAY               "Atraso"
#define TR_SD_CARD             "SD CARD"
#define TR_SDHC_CARD           "SD-HC CARD"
#define TR_NO_SOUNDS_ON_SD     "Sin Sonidos en SD"
#define TR_NO_MODELS_ON_SD     "Sin Modelos en SD"
#define TR_NO_BITMAPS_ON_SD    "Sin imagenes en SD"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Borrar"
#define TR_COPY_FILE           "Copiar"
#define TR_RENAME_FILE         "Renombrar"
#define TR_REMOVED             " borrado"
#define TR_SD_INFO             "Informacion"
#define TR_SD_FORMAT           "Formatear"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "COMPONENTES"
#define TR_FORMATTING          "Formateando."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Hora"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Tipo:"
#define TR_SD_SPEED            "Velocidad:"
#define TR_SD_SECTORS          "Sectores:"
#define TR_SD_SIZE             "Medida:"
#define TR_CURVE_TYPE          "Tipo"
#define TR_GLOBAL_VARS         "Global Variables"
#define TR_OWN                 "Propio"
#define TR_DATE                "Fecha"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "MONITOR CANALES"
