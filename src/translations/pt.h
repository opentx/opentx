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
#define TR_VBLMODE       "OFF ""Chav""Stks""Tudo""ON\0"

#define LEN_TRNMODE      "\003"
#define TR_TRNMODE       "OFF"" +="" :="

#define LEN_TRNCHN       "\003"
#define TR_TRNCHN        "CH1CH2CH3CH4"

#define LEN_DATETIME     "\005"
#define TR_DATETIME      "DATA:""HORA:"

#define LEN_VLCD         "\006"
#define TR_VLCD          "NormalOptrex"

#define LEN_VTRIMINC     "\006"
#define TR_VTRIMINC      "Expo  ""ExFino""Fino  ""Medio ""Largo "

#define LEN_RETA123      "\001"
#if defined(PCBGRUVIN9X)
#if ROTARY_ENCODERS > 2
#define TR_RETA123       "LPMA123abcd"
#else
#define TR_RETA123       "LPMA123ab"
#endif
#else
#define TR_RETA123       "LPMA123"
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

#define LEN_VMLTPX       "\011"
#define TR_VMLTPX        "Adicionar""Multipl. ""Trocar\0 0"

#define LEN_VMLTPX2      "\002"
#define TR_VMLTPX2       "+=""*="":="

#define LEN_VMIXTRIMS    "\003"
#define TR_VMIXTRIMS     "OFF""ON\0""Lem""Pfd""Mot""Ail"

#define LEN_VCSWFUNC     "\010"
#define TR_VCSWFUNC      "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""AND\0    ""OR\0     ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"

#define LEN_VFSWFUNC     "\015"
#if defined(VARIO)
#define TR_VVARIO        "Vario\0       "
#else
#define TR_VVARIO        "[Vario]\0     "
#endif
#if defined(AUDIO)
#define TR_SOUND         "Tocar Audio\0 "
#else
#define TR_SOUND         "Beep\0        "
#endif
#if defined(HAPTIC)
#define TR_HAPTIC        "Vibrar\0      "
#else
#define TR_HAPTIC        "[Vibrar]\0    "
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK    "Tocar Pista\0 "
#define TR_PLAY_VALUE    "Ouvir Valor\0 "
#else
#define TR_PLAY_TRACK    "[Tocar Pista]"
#define TR_PLAY_VALUE    "[Tocar Valor]"
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs\0 "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#define TR_FSW_VOLUME    "Volume\0      "
#define TR_FSW_BG_MUSIC  "BgMusic\0     ""BgMusic ||\0  "
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs\0 "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#else
#define TR_SDCLOGS
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#endif
#ifdef GVARS
#define TR_FSW_ADJUST_GVAR  "Ajuste\0      "
#else
#define TR_FSW_ADJUST_GVAR
#endif
#ifdef DEBUG
#define TR_FSW_TEST         "Teste\0       "
#else
#define TR_FSW_TEST
#endif

#define TR_VFSWFUNC      "Seguro\0      ""Aprendiz\0    ""Ajuste Rapido" TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Backlight\0   " TR_FSW_BG_MUSIC TR_FSW_ADJUST_GVAR TR_FSW_TEST

#define LEN_VFSWRESET    "\006"
#define TR_VFSWRESET     "Tempo1""Tempo2""Todo  ""Telem."

#define LEN_FUNCSOUNDS   "\006"
#define TR_FUNCSOUNDS    "Beep1 ""Beep2 ""Beep3 ""Avis1 ""Avis2 ""Pio  ""Ring  ""SciFi ""Robot ""Gorjeo""Tada  ""Crickt""Sirene""Alarme""Ratata""Tictac"

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
#define TR_VALARM        "---""Ama""Lar""Ver"

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
#define TR_VTEMPLATES    "Tirar Mixagem""4CH Simples\0 ""Anular Motor\0""V-Tail\0      ""Elevon\\Zagi\0 ""eCCPM\0       ""Mixagem Heli\0""Testar Servos"

#define LEN_VSWASHTYPE   "\004"
#define TR_VSWASHTYPE    "--- ""120 ""120X""140 ""90\0"

#define LEN_VKEYS        "\005"
#define TR_VKEYS         " Menu"" Sair""Desce"" Sobe""Direi""Esqda"

#define LEN_VRENCODERS   "\003"
#define TR_VRENCODERS    "REa""REb"

#define LEN_VSWITCHES    "\003"
#if defined(PCBSKY9X)
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CE1""CE2""CE3""CE4""CE5""CE6""CE7""CE8""CE9""CEA""CEB""CEC""CED""CEE""CEF""CEG""CEH""CEI""CEJ""CEK""CEL""CEM""CEN""CEO""CEP""CEQ""CER""CES""CET""CEU""CEV""CEW"" ON"
#else
#define TR_VSWITCHES     "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""CE1""CE2""CE3""CE4""CE5""CE6""CE7""CE8""CE9""CEA""CEB""CEC"" ON"
#endif

#define LEN_VSRCRAW      "\004"
#if defined(PCBSKY9X)
#define TR_ROTARY_ENCODERS_VSRCRAW "REa "
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
#define TR_VSRCRAW       "Lem\0""Pfd\0""Mot\0""Ail\0""Pot1""Pot2""Pot3" TR_ROTARY_ENCODERS_VSRCRAW "TrmL" "TrmP" "TrmM" "TrmA" "MAX\0""3POS" TR_CYC_VSRCRAW

#define LEN_VTMRMODES    "\003"
#define TR_VTMRMODES     "OFF""ABS""MTs""MT%""MTt"

#define LEN_DSM2MODE     "\007"
#define TR_DSM2MODE      "LP4/LP5DSMonlyDSMX   "

// ZERO TERMINATED STRINGS
#define INDENT                 "\001"
#define LEN_INDENT             1
#define INDENT_WIDTH           (FW/2)

#define TR_POPUPS              "[MENU]\010[SAIR] "
#define OFS_EXIT               7
#define TR_MENUWHENDONE        CENTER"\006[MENU]QDO PRONTO"
#define TR_FREE                "Livre"
#define TR_DELETEMODEL         "EXCLUI MODELO"
#define TR_COPYINGMODEL        "Copiando modelo"
#define TR_MOVINGMODEL         "Movendo modelo"
#define TR_LOADINGMODEL        "Carregar modelo"
#define TR_NAME                "Nome"
#define TR_TIMER               "Cronom"
#define TR_ELIMITS             "E.Limits"
#define TR_ETRIMS              "E.Trims"
#define TR_TRIMINC             "Trim Step"
#define TR_TTRACE              "T-Trace"
#define TR_TTRIM               "T-Trim"
#define TR_BEEPCTR             "Ctr Beep"
#define TR_PROTO               INDENT"Proto"
#define TR_PPMFRAME            "PPM frame"
#define TR_MS                  "ms"
#define TR_SWITCH              "Chave"
#define TR_BITMAP              "Bitmap"
#define TR_TRIMS               "Trims"
#define TR_FADEIN              "Aparecer"
#define TR_FADEOUT             "Ocultar"
#define TR_DEFAULT             "(default)"
#define TR_CHECKTRIMS          "\006Check\012Trims"
#define OFS_CHECKTRIMS         (9*FW)
#define TR_SWASHTYPE           "Ciclico Tipo"
#define TR_COLLECTIVE          "Coletivo"
#define TR_SWASHRING           "Anel Coletivo"
#define TR_ELEDIRECTION        "PFD Sentido"
#define TR_AILDIRECTION        "AIL Sentido"
#define TR_COLDIRECTION        "COL Sentido"
#define TR_MODE                INDENT"Modo"
#define TR_NOFREEEXPO          "No free expo!"
#define TR_NOFREEMIXER         "No free mixer!"
#define TR_INSERTMIX           "INSERIR MIX"
#define TR_EDITMIX             "EDITAR MIX"
#define TR_SOURCE              INDENT"Fonte"
#define TR_WEIGHT              "Quantia"
#define TR_EXPO                "Exponen"
#define TR_SIDE                "Lado"
#define TR_DIFFERENTIAL        "Diferencial"
#define TR_OFFSET              INDENT"Desvio"
#define TR_TRIM                "Trim"
#define TR_DREX                "DRex"
#define TR_CURVE               "Curvas"
#define TR_FPHASE              "Fase"
#define TR_MIXWARNING          "Aviso"
#define TR_OFF                 "OFF"
#define TR_MULTPX              "Aplicar"
#define TR_DELAYDOWN           "Atraso Dn"
#define TR_DELAYUP             "Atraso Up"
#define TR_SLOWDOWN            "Lento  Dn"
#define TR_SLOWUP              "Lento  Up"
#define TR_MIXER               "MIXAGENS"
#define TR_CV                  "CV"
#define TR_GV                  "GV"
#define TR_ACHANNEL            "A\004Canal"
#define TR_RANGE               INDENT"Range"
#define TR_BAR                 "Bar"
#define TR_ALARM               INDENT"Alarme"
#define TR_USRDATA             "UsrData"
#define TR_BLADES              INDENT"Helice"
#define TR_SCREEN              "Tela"
#define TR_SOUND_LABEL         "Som"
#define TR_LENGTH              INDENT"Comprimento"
#define TR_SPKRPITCH           INDENT"Pitch"
#define TR_HAPTIC_LABEL        "Vibrar"
#define TR_HAPTICSTRENGTH      INDENT"Forca"
#define TR_CONTRAST            "Contraste"
#define TR_ALARMS_LABEL        "Alarmes"
#define TR_BATTERYWARNING      INDENT"Bateria Baixa"
#define TR_INACTIVITYALARM     INDENT"Inactividade"
#define TR_MEMORYWARNING       INDENT"Memoria Baixa"
#define TR_ALARMWARNING        INDENT"Som Off"
#define TR_RENAVIG             "RotEnc Navig"
#define TR_THROTTLEREVERSE     "Inverte Acel."
#define TR_BEEP_LABEL          "Beep Cronom."
#define TR_MINUTEBEEP          INDENT"Beep Minuto"
#define TR_BEEPCOUNTDOWN       INDENT"Beep Regressivo"
#define TR_BACKLIGHT_LABEL     "Backlight"
#define TR_BLDELAY             INDENT"Tempo Backlight"
#define TR_SPLASHSCREEN        "Splash screen"
#define TR_THROTTLEWARNING     "Avisa Acel"
#define TR_SWITCHWARNING       "Avisa Chav"
#define TR_TIMEZONE            "Time Zone"
#define TR_RXCHANNELORD        "Ordem Canal RX"
#define TR_SLAVE               "Escravo"
#define TR_MODESRC             "Modo\006% Fonte"
#define TR_MULTIPLIER          "Multiplicar"
#define TR_CAL                 "Cal"
#define TR_VTRIM               "Trim- +"
#define TR_BG                  "BG:"
#define TR_MENUTOSTART         CENTER"\006[MENU] INICIAR"
#define TR_SETMIDPOINT         CENTER"\005CENTRAR STICK/POT"
#define TR_MOVESTICKSPOTS      CENTER"\005MOVER STICKS/POTs"
#define TR_RXBATT              "Rx Batt:"
#define TR_TXnRX               "Tx:\0Rx:"
#define OFS_RX                 4
#define TR_ACCEL               "Acc:"
#define TR_NODATA              "SEM DADOS"
#define TR_TM1TM2              "TM1\032TM2"
#define TR_THRTHP              "MTR\032MT%"
#define TR_TOT                 "TOT"
#define TR_TMR1LATMAXUS        "Tmr1Lat max\006us"
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS        "Tmr1Lat min\006us"
#define TR_TMR1JITTERUS        "Tmr1 Jitter\006us"
#if defined(PCBSKY9X)
#define TR_TMIXMAXMS           "Tmix max\012ms"
#else
#define TR_TMIXMAXMS           "Tmix max\014ms"
#endif
#define TR_T10MSUS             "T10ms\016us"
#define TR_FREESTACKMINB       "Pilha Livre\010b"
#define TR_MENUTORESET         "[MENU] Reinicia"
#define TR_PPM                 "PPM"
#define TR_CH                  "CH"
#define TR_MODEL               "MODEL"
#define TR_FP                  "FP"
#define TR_MIX                 "MIX"
#define TR_EEPROMLOWMEM        "EEPROM BX Memoria"
#define TR_ALERT               "\016ALERT"
#define TR_PRESSANYKEYTOSKIP   "Pulsar Tecla p/ sair"
#define TR_THROTTLENOTIDLE     "**ACELERADOR ATIVO**"
#define TR_ALARMSDISABLED      "Desabilita Alarmes"
#define TR_PRESSANYKEY         "\010Pressione Tecla"
#define TR_BADEEPROMDATA       "EEPRON INVALIDA"
#define TR_EEPROMFORMATTING    "Formatando EEPROM"
#define TR_EEPROMOVERFLOW      "EEPROM CHEIA"
#define TR_MENURADIOSETUP      "AJUSTAR RADIO"
#define TR_MENUDATEANDTIME     "DATA E HORA"
#define TR_MENUTRAINER         "TRAINER"
#define TR_MENUVERSION         "VERSAO"
#define TR_MENUDIAG            "DIAGNOST"
#define TR_MENUANA             "ANALOGICOS"
#define TR_MENUCALIBRATION     "CALIBRAGEM"
#define TR_TRIMS2OFFSETS       "\006Trims => Offsets"
#define TR_MENUMODELSEL        "MODELOS"
#define TR_MENUSETUP           "SETUP"
#define TR_MENUFLIGHTPHASE     "MODO DE VOO"
#define TR_MENUFLIGHTPHASES    "MODOS DE VOO"
#define TR_MENUHELISETUP       "HELI SETUP"
#if defined(PPM_CENTER_ADJUSTABLE) || defined(PPM_LIMITS_SYMETRICAL) // The right menu titles for the gurus ...
#define TR_MENUDREXPO          "STICKS"
#define TR_MENULIMITS          "SAIDAS"
#else
#define TR_MENUDREXPO          "DR/EXPO"
#define TR_MENULIMITS          "LIMITES"
#endif
#define TR_MENUCURVES          "CURVAS"
#define TR_MENUCURVE           "CURVA"
#define TR_MENUCUSTOMSWITCH    "CHAVE ESPECIAL"
#define TR_MENUCUSTOMSWITCHES  "CHAVES ESPECIAIS"
#define TR_MENUCUSTOMFUNC      "FUNCAO ESPECIAL"
#define TR_MENUTELEMETRY       "TELEMETRIA"
#define TR_MENUTEMPLATES       "MODELOS"
#define TR_MENUSTAT            "REGISTROS"
#define TR_MENUDEBUG           "DEPURAR"
#define TR_RXNUM               "RxNum"
#define TR_SYNCMENU            "Sync [MENU]"
#define TR_BACK                "Atras"
#define TR_LIMIT               INDENT"Limite"
#define TR_MINRSSI             "Min Rssi"
#define TR_LATITUDE            "Latitude"
#define TR_LONGITUDE           "Longitude"
#define TR_GPSCOORD            "Gps Coords"
#define TR_VARIO               "Vario"
#define TR_SHUTDOWN            "DESLIGANDOo"
#define TR_BATT_CALIB          "CALIBRAR BATT"
#define TR_CURRENT_CALIB       "Corrente Calib"
#define TR_VOLTAGE             INDENT"Volts"
#define TR_CURRENT             INDENT"Amperes"
#define TR_SELECT_MODEL        "Selec. Modelo"
#define TR_CREATE_MODEL        "Criar Modelo"
#define TR_BACKUP_MODEL        "Salvar Modelo"
#define TR_DELETE_MODEL        "Apagar Modelo"
#define TR_RESTORE_MODEL       "Restaura Modelo"
#define TR_SDCARD_ERROR        "SDCARD Erro"
#define TR_NO_SDCARD           "Sem SDCARD"
#define TR_INCOMPATIBLE        "Incompativel"
#define TR_WARNING             "AVISO"
#define TR_EEPROMWARN          "EEPROM"
#define TR_THROTTLEWARN        "ACELERAD"
#define TR_ALARMSWARN          "ALARMES"
#define TR_SWITCHWARN          "CHAVES"
#define TR_INVERT_THR          "Inverte Acel?"
#define TR_SPEAKER_VOLUME      INDENT"Volume"
#define TR_LCD                 "LCD"
#define TR_BRIGHTNESS          "Brilho"
#define TR_CPU_TEMP            "CPU Temp.\016>"
#define TR_CPU_CURRENT         "Current\022>"
#define TR_CPU_MAH             "Consumo."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB Temp. \016>"
#define TR_CAPAWARNING         INDENT"Aviso Capacidade"
#define TR_TEMPWARNING         INDENT"Temperat. ALTA"
#define TR_FUNC                "Funcao"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Tempo"
#define TR_DELAY               "Atraso"
#define TR_SD_CARD             "SD CARD"
#define TR_SDHC_CARD           "SD-HC CARD"
#define TR_NO_SOUNDS_ON_SD     "Sem Som no SD"
#define TR_NO_MODELS_ON_SD     "Sem Modelo no SD"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Apagar"
#define TR_COPY_FILE           "Copiar"
#define TR_RENAME_FILE         "Renomear"
#define TR_REMOVED             "Remover"
#define TR_SD_INFO             "Dados SD"
#define TR_SD_FORMAT           "Formatar"
#define TR_NA                  "N/A"
#define TR_HARDWARE            "HARDWARE"
#define TR_FORMATTING          "Formatando..."
#define TR_TEMP_CALIB          "Temp. Calib"
#define TR_NO_BITMAPS_ON_SD    "No Bitmaps on SD"
#define TR_TIME                "Time"
#define TR_BAUDRATE            "BT Baudrate"
#define TR_SD_INFO_TITLE       "SD INFO"
#define TR_SD_TYPE             "Tipe"
#define TR_SD_SPEED            "Velocidade"
#define TR_SD_SECTORS          "Sectores"
#define TR_SD_SIZE             "Tamanho"
#define TR_CURVE_TYPE          "Type"
#define TR_GLOBAL_VARS         "Variaveis"
#define TR_OWN                 "Propr."
#define TR_DATE                "Data"
#define TR_ROTARY_ENCODER      "R.Encs"
#define TR_CHANNELS_MONITOR    "CHANNELS MONITOR"
