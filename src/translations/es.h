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

#define LEN_VFILTERADC   "\004"
#define TR_VFILTERADC    "SIMP""OSMP""FILT"

#define LEN_VBLMODE      "\004"
#define TR_VBLMODE       "OFF ""Keys""Stks""Dos ""ON\0"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "off"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "ch1ch2ch3ch4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATA:""HORA:"

#define LEN_VLCD         "\006"
#define TR_VLCD          "NormalOptrex"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Expo ""ExFino""Fino  ""Medio ""Grueso"

#define LEN_RETA123      "\001"
#if defined(PCBGRUVIN9X)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123       "RETA123abcd"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123       "RETA123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123       "RETA123"
#endif

#define LEN_VPROTOS      "\006"
#ifdef PXX
#define TR_PXX  "PXX\0  "
#else
#define TR_PXX  "[PXX]\0"
#endif
#ifdef DSM2
#define TR_DSM2 "DSM2\0 "
#else
#define TR_DSM2 "[DSM2]"
#endif
#ifdef IRPROTOS
#define TR_IRPROTOS "SILV  TRAC09PICZ  SWIFT\0"
#else
#define TR_IRPROTOS
#endif
#define TR_VPROTOS       "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2 TR_IRPROTOS

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

#define LEN_VCSWFUNC     "\010"
#define TR_VCSWFUNC      "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""AND\0    ""OR\0     ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"

#define LEN_VFSWFUNC     "\015"
#if defined(VARIO)
#define TR_VVARIO        "Vario        "
#else
#define TR_VVARIO        "[Vario]      "
#endif
#if defined(AUDIO)
#define TR_SOUND         "Oir Sonido\0  "
#else
#define TR_SOUND         "Beep\0        "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC        "Vibrar\0      "
#else
#define TR_HAPTIC        "[Vibrar]\0    "
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK    "Oir Pista \0  "
#define TR_PLAY_VALUE    "Oir Valor \0  "
#else
#define TR_PLAY_TRACK    "[Oir Pista] \0"
#define TR_PLAY_VALUE    "[Oir Valor] \0"
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logo\0 "
#else
#define TR_SDCLOGS       "[SDCARD Logo]"
#endif
#define TR_FSW_VOLUME    "Volumen\0      "
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logo  "
#else
#define TR_SDCLOGS       "[SDCARD Logo]"
#endif
#define TR_FSW_VOLUME
#else
#define TR_SDCLOGS
#define TR_FSW_VOLUME

#endif
#ifdef DEBUG
#define TR_TEST          "Test\0"
#else
#define TR_TEST
#endif
#define TR_VFSWFUNC      "Seguro\0      ""Aprendiz \0    ""Ajuste Rapido" TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Panel Luz\0   " TR_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Reloj1""Reloj2""Todo  ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Pito1 ""Pito2 ""Pito3 ""Avis1 ""Avis2 ""Pio  ""Ring  ""SciFi ""Robot ""Gorjeo""Tada  ""Crickt""Sirena""Alarma""Ratata""Tictac"

#define LEN_VTELEMCHNS   "\004"
#define TR_VTELEMCHNS    "---\0""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VVel""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Vel+""Dst+""Cur+""Acc\0""Hora"

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
#define TR_VTELPROTO     "Nada\0  ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC      "\003"
#define TR_VOLTSRC       "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC     "\004"
#define TR_VARIOSRC      "Data""A1\0 ""A2\0"

#define LEN_VSCREEN      "\004"
#define TR_VSCREEN       "Nums""Bars"

#define LEN_GPSFORMAT    "\004"
#define TR_GPSFORMAT     "HMS NMEA"

#define LEN2_VTEMPLATES  13
#define LEN_VTEMPLATES   "\015"
#define TR_VTEMPLATES    "Elim Mezcla\0\0""Simple 4-CH \0""Anular Motor\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu""Salir""Abajo""Arrib""Drcha"" Izqda"

#define LEN_VRENCODERS   "\003"
#define TR_VRENCODERS    "REa""REb"

#define LEN_VSWITCHES    "\003"
#if defined(PCBSKY9X)
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC""CSD""CSE""CSF""CSG""CSH""CSI""CSJ""CSK""CSL""CSM""CSN""CSO""CSP""CSQ""CSR""CSS""CST""CSU""CSV""CSW"" ON"
#else
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CS1""CS2""CS3""CS4""CS5""CS6""CS7""CS8""CS9""CSA""CSB""CSC"" ON"
#endif

#define LEN_VSRCRAW      "\004"
#if defined(PCBSKY9X)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa "
#elif defined(PCBGRUVIN9X) && defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb ""REc ""REd "
#elif defined(PCBGRUVIN9X) && !defined(EXTRA_ROTARY_ENCODERS)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa ""REb "
#else
#define TR_ROTARY_ENCODERS_VSRCRAW
#endif
#if defined(HELI)
#define TR_CYC_VSRCRAW   "CYC1""CYC2""CYC3"
#else
#define TR_CYC_VSRCRAW   "[C1]""[C2]""[C3]"
#endif
#define TR_VSRCRAW       "Dir ""Ele ""Acel""Ail ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" "MAX ""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""THs""TH%""THt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#define TR_POPUPS              "[MENU]\010[SALIR]"
#define OFS_EXIT               7
#define TR_MENUWHENDONE        "[MENU]AL FINAL"
#define TR_FREE                "Libre"
#define TR_DELETEMODEL         "BORRA MODELO"
#define TR_COPYINGMODEL        "Copiando modelo."
#define TR_MOVINGMODEL         "Mover modelo..."
#define TR_LOADINGMODEL        "Cargar modelo..."
#define TR_NAME                "Nombre"
#define TR_TIMER               "Reloj"
#define TR_ELIMITS             "F.Limites"
#define TR_ETRIMS              "F.Ajustes"
#define TR_TRIMINC             "Modo Ajte"
#define TR_TTRACE              "T-Rastro"
#define TR_TTRIM               "T-Ajuste"
#define TR_BEEPCTR             "Pitido en"
#define TR_PROTO               INDENT"Protocolo"
#define TR_PPMFRAME            "Medida PPM"
#define TR_MS                  "ms"
#define TR_SWITCH              "Llave"
#define TR_TRIMS               "Ajustes"
#define TR_FADEIN              "Aparecer"
#define TR_FADEOUT             "Ocultar"
#define TR_DEFAULT             "(p/defecto)"
#define TR_CHECKTRIMS          "\006Check\012Ajtes"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Ciclico Tipo"
#define TR_COLLECTIVE          "Colectivo"
#define TR_SWASHRING           "Anillo Cvo"
#define TR_ELEDIRECTION        "ELE Direccion"
#define TR_AILDIRECTION        "ALE Direccion"
#define TR_COLDIRECTION        "COL Direccion"
#define TR_MODE                "Modo"
#define TR_NOFREEEXPO          "No free expo!"
#define TR_NOFREEMIXER         "No free mixer!"
#define TR_INSERTMIX           "INSERT MIX "
#define TR_EDITMIX             "EDIT MIX "
#define TR_SOURCE              INDENT"Fuente"
#define TR_WEIGHT              "Cantidad"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Lado"
#define TR_DIFFERENTIAL        "Diferir"
#define TR_OFFSET              INDENT"Offset"
#define TR_TRIM                "Ajte"
#define TR_DREX                "DRex"
#define TR_CURVE               "Curva"
#define TR_FPHASE              "Fase"
#define TR_MIXWARNING          "Aviso"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "Multipl"
#define TR_DELAYDOWN           "Delay Dn"
#define TR_DELAYUP             "Delay Up"
#define TR_SLOWDOWN            "Slow  Dn"
#define TR_SLOWUP              "Slow  Up"
#define TR_MIXER               "MEZCLA"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004Canal"
#define TR_RANGE               INDENT"Range"
#define TR_BAR                 "Bar"
#define TR_ALARM               INDENT"Alarma"
#define TR_USRDATA             "UsrData"
#define TR_BLADES              INDENT"Palas"
#define TR_SCREEN              "Screen "
#define TR_SOUND_LABEL         "Sound"
#define TR_LENGTH              INDENT"Length"
#define TR_SPKRPITCH           "Speaker Pitch"
#define TR_HAPTIC_LABEL        "Haptic"
#define TR_HAPTICSTRENGTH      INDENT"Strength"
#define TR_CONTRAST            "Contraste"
#define TR_ALARMS_LABEL        "Alarms"
#define TR_BATTERYWARNING      "Aviso Bateria"
#define TR_INACTIVITYALARM     "Alarma Reposo"
#define TR_RENAVIG             "Navig RotEnc"
#define TR_FILTERADC           "Filtro ADC"
#define TR_THROTTLEREVERSE     "Inversion Acel."
#define TR_BEEP_LABEL          "Timer events"
#define TR_MINUTEBEEP          "Pitido/minuto"
#define TR_BEEPCOUNTDOWN       "Pitido ctaatras"
#define TR_BACKLIGHT_LABEL     "Backlight"
#define TR_FLASHONBEEP         "Flash al pitido"
#define TR_BLMODE              "Modo P.Luminoso"
#define TR_BLDELAY             "Tiempo de Luz"
#define TR_SPLASHSCREEN        "Pantalla Inicio"
#define TR_THROTTLEWARNING     "Aviso Acelerdor"
#define TR_SWITCHWARNING       "Aviso de Llaves"
#define TR_MEMORYWARNING       "Aviso Memoria"
#define TR_ALARMWARNING        "Aviso de alarma"
#define TR_TIMEZONE            "Zona Horaria"
#define TR_RXCHANNELORD        "Rx Ord Canal"
#define TR_SLAVE               "Esclavo"
#define TR_MODESRC             "modo\006% src"
#define TR_MULTIPLIER          "Multiplicar"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Ajte- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         "[MENU] INICIAR"
#define TR_SETMIDPOINT         "AJUSTAR CENTROS"
#define TR_MOVESTICKSPOTS      "MOVER STICK/POT"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              "si"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "THR\032TH%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"
#if defined(PCBSKY9X)
#define TR_TMAINMAXMS          "Tmain max\010ms"
#else
#define TR_TMAINMAXMS          "Tmain max\012ms"
#endif
#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Pila libre\010b"
#define TR_MENUTORESET         "[MENU] Reinicia"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODELO"
#define TR_FP                  "FP"
#define TR_EEPROMLOWMEM        "EEPROM Baja mem"
#define TR_ALERT               "\016ALERTA"
#define TR_PRESSANYKEYTOSKIP   "Pulsa tecla x omitir"
#define TR_THROTTLENOTIDLE     "Acelerador activado"
#define TR_ALARMSDISABLED      "Alarmas Desact."
#define TR_PRESSANYKEY         "\010Pulse una tecla"
#define TR_BADEEPROMDATA       "Datos EEprom mal"
#define TR_EEPROMFORMATTING    "Formateo EEPROM"
#define TR_EEPROMOVERFLOW      "Desborde EEPROM"
#define TR_MENURADIOSETUP      "AJUSTAR RADIO"
#define TR_MENUDATEANDTIME     "FECHA Y HORA"
#define TR_MENUTRAINER         "APRENDIZAJE"
#define TR_MENUVERSION         "VERSION"
#define TR_MENUDIAG            "DIAGNOST."
#define TR_MENUANA             "ANALOGICOS"
#define TR_MENUCALIBRATION     "CALIBRACION"
#define TR_TRIMS2OFFSETS       "Trims => Offsets"
#define TR_MENUMODELSEL        "MODELSEL"
#define TR_MENUSETUP           "AJUSTE"
#define TR_MENUFLIGHTPHASE     "FASE DE VUELO"
#define TR_MENUFLIGHTPHASES    "FASES DE VUELO"
#define TR_MENUHELISETUP       "AJUSTE HELI"
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
#define TR_MENUDREXPO          "STICKS"
#define TR_MENULIMITS          "SALIDAS"
#else
#define TR_MENUDREXPO          "DR/EXPO"
#define TR_MENULIMITS          "LIMITES"
#endif
#define TR_MENUCURVES          "CURVAS"
#define TR_MENUCURVE           "CURVA"
#define TR_MENUCUSTOMSWITCH    "AJUSTES PROPIOS"
#define TR_MENUCUSTOMSWITCHES  "AJUSTE PROPIO"
#define TR_MENUCUSTOMFUNC    "AJUSTAR FUNCIONES"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "PLANTILLAS"
#define TR_MENUSTAT            "REGISTRO"
#define TR_MENUDEBUG           "DEPURAR"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_BACK                "Atras"
#define TR_LIMIT               INDENT "Limit"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitud"
#define TR_LONGITUDE           "Longitud"
#define TR_GPSCOORD            "Gps Coords"
#define TR_VARIO               "Vario"
#define TR_SHUTDOWN            "APAGANDO"
#define TR_BATT_CALIB          "Bateria Calib"
#define TR_CURRENT_CALIB       "Actual Calib"
#define TR_VOLTAGE             "Voltaje"
#define TR_CURRENT             "Actual"
#define TR_SELECT_MODEL        "Selec Modelo"
#define TR_CREATE_MODEL        "Crear Modelo"
#define TR_BACKUP_MODEL        "Salvar Model"
#define TR_DELETE_MODEL        "Borrar Model"
#define TR_RESTORE_MODEL       "Recupr.Model"
#define TR_SDCARD_ERROR        "SDCARD Error"
#define TR_NO_SDCARD           "No SDCARD"
#define TR_INCOMPATIBLE        "Inconpatible"
#define TR_WARNING             "AVISO"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "ACELERAD"
#define TR_ALARMSWARN          "ALARMAS"
#define TR_SWITCHWARN          "LLAVE"
#define TR_INVERT_THR          "Invert acel?"
#define TR_SPEAKER_VOLUME      "Volume Altavoz"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Brillo"
#define TR_CPU_TEMP            "CPU Temp.\016>"
#define TR_CPU_CURRENT         "Actual\022>"
#define TR_CPU_MAH             "Consum."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT "Capacidad Low"
#define TR_TEMPWARNING         INDENT "Temperat High"
#define TR_FUNC                "Funcion"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duracion"
#define TR_DELAY               "Retardo"
#define TR_SD_CARD             "SD Card"
#define TR_SDHC_CARD           "SD-HC Card"
#define TR_NO_SOUNDS_ON_SD     "No Sonido en SD"
#define TR_NO_MODELS_ON_SD     "No Modelos en SD"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Borrar"
#define TR_COPY_FILE           "Copiar"
#define TR_RENAME_FILE         "Renomb"
#define TR_REMOVED             "borrado"
#define TR_SD_INFO             "Infos"
#define TR_SD_FORMAT           "Formateo"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatting..."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_TIME                "Time"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Type:"
#define TR_SD_SPEED            "Speed:"
#define TR_SD_SECTORS          "Sectors:"
#define TR_SD_SIZE             "Size:"
#define TR_CURVE_TYPE          "Type"
#define TR_GLOBAL_VARS         "Global Variables"
#define TR_OWN                 "Own"
