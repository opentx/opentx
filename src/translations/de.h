// NON ZERO TERMINATED STRINGS
#define LEN_OFFON                       "\003"                                                                                                        
#define TR_OFFON                        "AUS""AN\0"                                                                                                    
                                                                                                                                                      
#define LEN_MMMINV                      "\003"                                                                                                        
#define TR_MMMINV                       "---""INV"                                                                                                    
                                                                                                                                                      
#define LEN_NCHANNELS                   "\004"                                                                                                        
#define TR_NCHANNELS                    "4CH 6CH 8CH 10CH12CH14CH16CH"                                                                                
                                                                                                                                                      
#define LEN_VBEEPMODE                   "\005"                                                                                                        
#define TR_VBEEPMODE                    "Stumm""Alarm""NoKey""Alle "                                                                                  

#define LEN_VBEEPLEN                    "\005"
#define TR_VBEEPLEN                     "0====""=0===""==0==""===0=""====0"

#define LEN_VRENAVIG                    "\003"                                                                                                        
#define TR_VRENAVIG                     "AUSREaREb"                                                                                                   
                                                                                                                                                      
#define LEN_VFILTERADC                  "\004"                                                                                                        
#define TR_VFILTERADC                   "SING""OSMP""FILT"                                                                                            
                                                                                                                                                      
#define LEN_VBLMODE      "\004"
#define TR_VBLMODE       "OFF ""Keys""Stks""Both""ON\0"

#define LEN_TRNMODE                     "\003"                                                                                                        
#define TR_TRNMODE                      "AUS"" +="" :="                                                                                               
                                                                                                                                                      
#define LEN_TRNCHN                      "\003"                                                                                                        
#define TR_TRNCHN                       "CH1CH2CH3CH4"                                                                                                
                                                                                                                                                      
#define LEN_DATETIME                    "\005"                                                                                                        
#define TR_DATETIME                     "DATE:""TIME:"                                                                                                
                                                                                                                                                      
#define LEN_VTRIMINC                    "\006"                                                                                                        
#define TR_VTRIMINC                     "Expo. ""X-Fein""Fein  ""Mittel""Grob  "                                                                      
                                                                                                                                                      
#define LEN_RETA123                     "\001"                                                                                                        
#if defined(PCBGRUVIN9X)
#if defined(EXTRA_ROTARY_ENCODERS)
#define TR_RETA123                      "RETA123abcd"
#else //EXTRA_ROTARY_ENCODERS
#define TR_RETA123                      "RETA123ab"
#endif //EXTRA_ROTARY_ENCODERS
#else
#define TR_RETA123                      "RETA123"                                                                                                                                                                                                                                                  
#endif                                                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                   
#define LEN_VPROTOS                     "\006"                                                                                                                                                                                                                                                     
#ifdef PXX                                                                                                                                                                                                                                                                                         
#define TR_PXX                          "PXX\0  "                                                                                                                                                                                                                                                                          
#else                                                                                                                                                                                                                                                                                              
#define TR_PXX                          "[PXX]\0"                                                                                                                                                                                                                                                                          
#endif                                                                                                                                                                                                                                                                                             
#ifdef DSM2                                                                                                                                                                                                                                                                                        
#define TR_DSM2                         "DSM2\0 "                                                                                                                                                                                                                                                                          
#else                                                                                                                                                                                                                                                                                              
#define TR_DSM2                         "[DSM2]"                                                                                                                                                                                                                                                                           
#endif                                                                                                                                                                                                                                                                                             
#ifdef IRPROTOS
#define TR_IRPROTOS                     "SILV  TRAC09PICZ  SWIFT\0"
#else
#define TR_IRPROTOS
#endif
#define TR_VPROTOS                      "PPM\0  ""PPM16\0""PPMsim" TR_PXX TR_DSM2                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                                   
#define LEN_POSNEG                      "\003"                                                                                                                                                                                                                                                     
#define TR_POSNEG                       "POS""NEG"                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                   
#define LEN_VCURVEFUNC                  "\003"                                                                                                                                                                                                                                                     
#define TR_VCURVEFUNC                   "---""x>0""x<0""|x|""f>0""f<0""|f|"                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                   
#define LEN_VMLTPX                      "\010"                                                                                                                                                                                                                                                     
#define TR_VMLTPX                       "Add     ""Multiply""Replace "                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                   
#define LEN_VMLTPX2                     "\002"                                                                                                                                                                                                                                                     
#define TR_VMLTPX2                      "+=""*="":="                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                                   
#define LEN_VMIXTRIMS                   "\003"                                                                                                                                                                                                                                                     
#define TR_VMIXTRIMS                    "AUS""AN ""Rud""Ele""Thr""Ail"                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                   
#define LEN_VCSWFUNC                    "\010"
#define TR_VCSWFUNC                     "---\0    ""v>ofs\0  ""v<ofs\0  ""|v|>ofs\0""|v|<ofs\0""AND\0    ""OR\0     ""XOR\0    ""v1==v2\0 ""v1!=v2\0 ""v1>v2\0  ""v1<v2\0  ""v1>=v2\0 ""v1<=v2\0 ""d>=ofs\0 ""|d|>=ofs"
                                                                                                                                                                                                                                                                                                   
#define LEN_VFSWFUNC                    "\015"                                                                                                                                                                                                                                                     
#if defined(VARIO)                                                                                                                                                                                                                                                                                 
#define TR_VVARIO                       "Vario        "                                                                                                                                                                                                                                            
#else                                                                                                                                                                                                                                                                                              
#define TR_VVARIO                       "[Vario]      "                                                                                                                                                                                                                                            
#endif                                                                                                                                                                                                                                                                                             
#if defined(AUDIO)                                                                                                                                                                                                                                                                                 
#define TR_SOUND                        "Play Sound\0  "                                                                                                                                                                                                                                           
#else                                                                                                                                                                                                                                                                                              
#define TR_SOUND                        "Beep\0        "                                                                                                                                                                                                                                           
#endif                                                                                                                                                                                                                                                                                             
#if defined(HAPTIC)                                                                                                                                                                                                                                                                                
#define TR_HAPTIC                       "Haptic\0      "                                                                                                                                                                                                                                           
#else                                                                                                                                                                                                                                                                                              
#define TR_HAPTIC                       "[Haptic]\0    "                                                                                                                                                                                                                                           
#endif
#if defined(VOICE)
#define TR_PLAY_TRACK                   "Play Track\0  "
#define TR_PLAY_VALUE                   "Play Value\0  "
#else
#define TR_PLAY_TRACK                   "[Play Track]\0"
#define TR_PLAY_VALUE                   "[Play Value]\0"
#endif
#if defined(PCBSKY9X)
#if defined(SDCARD)
#define TR_SDCLOGS       "SDCARD Logs\0 "
#else
#define TR_SDCLOGS       "[SDCARD Logs]"
#endif
#define TR_FSW_VOLUME    "Volume\0      "
#define TR_FSW_BG_MUSIC  "Bg Music\0    "
#elif defined(PCBGRUVIN9X)
#if defined(SDCARD)                                                                                                                                                                                                                                                                                
#define TR_SDCLOGS                      "SDcard Logs  "                                                                                                                                                                                                                                            
#else                                                                                                                                                                                                                                                                                              
#define TR_SDCLOGS                      "[SDcard Logs]"                                                                                                                                                                                                                                            
#endif                                                                                                                                                                                                                                                                                             
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#else                                                                                                                                                                                                                                                                                              
#define TR_SDCLOGS                                                                                                                                                                                                                                                                                 
#define TR_FSW_VOLUME
#define TR_FSW_BG_MUSIC
#endif                                                                                                                                                                                                                                                                                             
#ifdef DEBUG                                                                                                                                                                                                                                                                                       
#define TR_TEST                         "Test\0        "                                                                                                                                                                                                                                           
#else                                                                                                                                                                                                                                                                                              
#define TR_TEST                                                                                                                                                                                                                                                                                    
#endif                                                                                                                                                                                                                                                                                             
#define TR_VFSWFUNC                     "Safety\0      ""Trainer \0    ""Instant Trim " TR_SOUND TR_HAPTIC "Reset\0       " TR_VVARIO TR_PLAY_TRACK TR_PLAY_VALUE TR_SDCLOGS TR_FSW_VOLUME "Backlight\0   " TR_FSW_BG_MUSIC TR_TEST
                                                                                                                                                                                                                                                                                                   
#define LEN_VFSWRESET                   "\006"                                                                                                                                                                                                                                                     
#define TR_VFSWRESET                    "Timer1""Timer2""Alle  ""Telem."                                                                                                                                                                                                                           
                                                                                                                                                                                                                                                                                                   
#define LEN_FUNCSOUNDS                  "\006"                                                                                                                                                                                                                                                     
#define TR_FUNCSOUNDS                   "Beep1 ""Beep2 ""Beep3 ""Warn 1""Warn 2""Cheep ""Ring  ""SciFi ""Robot ""Chirp ""Tada  ""Crickt""Siren ""AlmClk""Ratata""Tick  "                                                                                                                                                   
                                                                                                                                                                                                                                                                                                   
#define LEN_VTELEMCHNS                  "\004"                                                                                                                                                                                                                                                     
#define TR_VTELEMCHNS                   "---\0""Tmr1""Tmr2""Tx\0 ""Rx\0 ""A1\0 ""A2\0 ""Alt\0""Rpm\0""Fuel""T1\0 ""T2\0 ""Spd\0""Dist""GAlt""Cell""Cels""Vfas""Curr""Cnsp""Powr""AccX""AccY""AccZ""Hdg\0""VSpd""A1-\0""A2-\0""Alt-""Alt+""Rpm+""T1+\0""T2+\0""Spd+""Dst+""Cur+""Acc\0""Time"
                                                                                                                                                                                                                                                                                                   
#ifdef IMPERIAL_UNITS                                                                                                                                                                                                                                                                              
#define LENGTH_UNIT                     "ft\0"                                                                                                                                                                                                                                                     
#define SPEED_UNIT                      "kts"                                                                                                                                                                                                                                                      
#else                                                                                                                                                                                                                                                                                              
#define LENGTH_UNIT                     "m\0 "                                                                                                                                                                                                                                                     
#define SPEED_UNIT                      "kmh"                                                                                                                                                                                                                                                      
#endif                                                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                   
#define LEN_VTELEMUNIT                  "\003"                                                                                                                                                                                                                                                     
#define TR_VTELEMUNIT                   "v\0 ""A\0 ""m/s""-\0 " SPEED_UNIT LENGTH_UNIT "@\0 ""%\0 ""mA\0""mAh""W\0 "
#define STR_V                           (STR_VTELEMUNIT+1)                                                                                                                                                                                                                                         
#define STR_A                           (STR_VTELEMUNIT+4)                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                                                                                   
#define LEN_VALARM                      "\003"                                                                                                                                                                                                                                                     
#define TR_VALARM                       "---""Yel""Org""Red"                                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                                                   
#define LEN_VALARMFN                    "\001"                                                                                                                                                                                                                                                     
#define TR_VALARMFN                     "<>"                                                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                                                                   
#define LEN_VTELPROTO    "\007"
#define TR_VTELPROTO     "---\0   ""Hub\0   ""WSHHigh""Halcyon"

#define LEN_VOLTSRC      "\003"
#define TR_VOLTSRC       "---""A1\0""A2\0""FAS""Cel"

#define LEN_VARIOSRC     "\004"
#define TR_VARIOSRC      "Data""A1\0 ""A2\0"
                                                                                                                                                                                                                                                                                                   
#define LEN_GPSFORMAT                   "\004"                                                                                                                                                                                                                                                     
#define TR_GPSFORMAT                    "HMS NMEA"                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                   
#define LEN2_VTEMPLATES                 13                                                                                                                                                                                                                                                         
#define LEN_VTEMPLATES                  "\015"                                                                                                                                                                                                                                                     
#define TR_VTEMPLATES                   "Clear Mixes\0\0""Simple 4-CH \0""Sticky-T-Cut\0""V-Tail      \0""Elevon\\Delta\0""eCCPM       \0""Heli Setup  \0""Servo Test  \0"                                                                                                                         
                                                                                                                                                                                                                                                                                                   
#define LEN_VSWASHTYPE                  "\004"                                                                                                                                                                                                                                                     
#define TR_VSWASHTYPE                   "--- ""120 ""120X""140 ""90\0"
                                                                                                                                                                                                                                                                                                   
#define LEN_VKEYS                       "\005"                                                                                                                                                                                                                                                     
#define TR_VKEYS                        " Menu"" Exit""Unten"" Oben""  Re.""  Li."                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                   
#define LEN_VRENCODERS                      "\003"                                                                                                                                                                                                                                                     
#define TR_VRENCODERS                       "REa""REb"                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                   
#define LEN_VSWITCHES                   "\003"                                                                                                                                                                                                                                                     
#if defined(PCBSKY9X)                                                                                                                                                                                                                                                                                
#define TR_VSWITCHES                    "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC""SWD""SWE""SWF""SWG""SWH""SWI""SWJ""SWK""SWL""SWM""SWN""SWO""SWP""SWQ""SWR""SWS""SWT""SWU""SWV""SWW"" ON"
#else                                                                                                                                                                                                                                                                                              
#define TR_VSWITCHES                    "THR""RUD""ELE""ID0""ID1""ID2""AIL""GEA""TRN""SW1""SW2""SW3""SW4""SW5""SW6""SW7""SW8""SW9""SWA""SWB""SWC"" ON"
#endif                                                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                   
#define LEN_VSRCRAW                     "\004"                                                                                                                                                                                                                                                     
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
#define TR_CYC_VSRCRAW                  "CYC1""CYC2""CYC3"                                                                                                             
#else                                                                                                                                                                  
#define TR_CYC_VSRCRAW                  "[C1]""[C2]""[C3]"                                                                                                             
#endif                                                                                                                                                                 
#define TR_VSRCRAW                      "Rud ""Ele ""Thr ""Ail ""P1  ""P2  ""P3  " TR_ROTARY_ENCODERS_VSRCRAW "TrmR" "TrmE" "TrmT" "TrmA" "MAX ""3POS" TR_CYC_VSRCRAW  
                                                                                                                                                                       
#define LEN_VTMRMODES                   "\003"                                                                                                                         
#define TR_VTMRMODES                    " \210 ""ABS""THs""TH%""THt"                                                                                                      
                                                                                                                                                                       
#define LEN_DSM2MODE                    "\007"                                                                                                                         
#define TR_DSM2MODE                     "LP4/LP5DSMonlyDSMX   "                                                                                                        

// ZERO TERMINATED STRINGS
#define TR_POPUPS                       "[MENU]\004[EXIT]"                                                    
#define OFS_EXIT                        7                                                                     
#define TR_MENUWHENDONE                 "[MENU] > Weiter"                                                     
#define TR_FREE                         "frei"                                                                
#define TR_DELETEMODEL                  "Modell l\203schen?"                                                  
#define TR_COPYINGMODEL                 "Kopiere Modell..."                                                   
#define TR_MOVINGMODEL                  "Schiebe Modell..."                                                   
#define TR_LOADINGMODEL                 "Lade Modell..."                                                      
#define TR_NAME                         "Name"                                                                
#define TR_TIMER                        "Timer "                                                              
#define TR_ELIMITS                      "E.Limits"                                                            
#define TR_ETRIMS                       "E.Trims"                                                             
#define TR_TRIMINC                      "Trim Inc"                                                            
#define TR_TTRACE                       "T-Trace"                                                             
#define TR_TTRIM                        "T-Trim"                                                              
#define TR_BEEPCTR                      "Beep Ctr"                                                            
#define TR_PROTO                        "Protokoll"                                                           
#define TR_PPMFRAME                     "PPM frame"                                                           
#define TR_MS                           "ms"                                                                  
#define TR_SWITCH                       "Switch"                                                              
#define TR_TRIMS                        "Trims"                                                               
#define TR_FADEIN                       "Fade In"                                                             
#define TR_FADEOUT                      "Fade Out"                                                            
#define TR_DEFAULT                      "(Standard)"                                                           
#define TR_CHECKTRIMS                   "Check\005Trims"                                                      
#define TR_SWASHTYPE                    "Swash Type"                                                          
#define TR_COLLECTIVE                   "Collective"                                                          
#define TR_SWASHRING                    "Swash Ring"                                                          
#define TR_ELEDIRECTION                 "ELE Direction"                                                       
#define TR_AILDIRECTION                 "AIL Direction"                                                       
#define TR_COLDIRECTION                 "COL Direction"                                                       
#define TR_MODE                         "Mode"                                                                
#define TR_NOFREEEXPO                   "No free expo!"                                                       
#define TR_NOFREEMIXER                  "No free mixer!"                                                      
#define TR_INSERTMIX                    "INSERT MIX "                                                         
#define TR_EDITMIX                      "MIXER "                                                            
#define TR_SOURCE                       "Quelle"                                                              
#define TR_WEIGHT                       "Gewicht"
#define TR_EXPO                "Expo"
#define TR_SIDE                "Side"
#define TR_MIXERWEIGHT                  "Mixer Weight"                                                        
#define TR_DIFFERENTIAL                 "Differ"                                                              
#define TR_OFFSET                       "Offset"                                                              
#define TR_MIXEROFFSET                  "Mixer Offset"                                                        
#define TR_DRWEIGHT                     "DR Weight"                                                           
#define TR_DREXPO                       "DR Expo"                                                             
#define TR_TRIM                         "Trim"
#define TR_DREX                         "DRex"                                                                
#define TR_CURVE                        "Kurve"
#define TR_FPHASE                       "Phase"
#define TR_MIXWARNING                   "Warnung"
#define TR_OFF                          "AUS"                                                                 
#define TR_MULTPX                       "Multpx"                                                              
#define TR_DELAYDOWN                    "Verz. Unten"                                                         
#define TR_DELAYUP                      "Verz. Oben"                                                          
#define TR_SLOWDOWN                     "Langs. Unten"                                                        
#define TR_SLOWUP                       "Langs. Oben"                                                         
#define TR_MIXER                        "MIXER"                                                               
#define TR_CV                           "CV"                                                                  
#define TR_ACHANNEL                     "A\002channel"                                                        
#define TR_RANGE                        "Range"                                                               
#define TR_BAR                          "Bar"                                                                 
#define TR_ALARM                        "Alarm"                                                               
#define TR_USRDATA                      "UsrData"                                                             
#define TR_BLADES                       "Blades"                                                              
#define TR_BARS                         "Bars"                                                                
#define TR_DISPLAY                      "Display"                                                             
#ifdef AUDIO                                                                                                  
#define TR_BEEPERMODE                   "Speaker Modus"                                                       
#define TR_BEEPERLEN                    "Speaker L\201nge"                                                    
#define TR_SPKRPITCH                    "Speaker H\202he"                                                     
#else                                                                                                         
#define TR_BEEPERMODE                   "Pieper Modus"                                                        
#define TR_BEEPERLEN                    "Pieper L\201nge"                                                     
#endif                                                                                                        
#define TR_HAPTICMODE                   "Haptic Mode"                                                         
#define TR_HAPTICSTRENGTH               "Haptic Strength"                                 
#define TR_HAPTICLENGTH                 "Haptic Length"                                   
#define TR_CONTRAST                     "Displ. Kontrast"                                 
#define TR_BATTERYWARNING               "Alarm Batterie"                                  
#define TR_INACTIVITYALARM              "Alarm Inaktiv"                                   
#define TR_RENAVIG                      "Navig RotEnc"                                    
#define TR_FILTERADC                    "Filter ADC"                                      
#define TR_THROTTLEREVERSE              "THR Invers"                                      
#define TR_MINUTEBEEP                   "Pieper Minuten"                                  
#define TR_BEEPCOUNTDOWN                "Pieper Countdown"                                
#define TR_FLASHONBEEP                  "Pieper+Blinken"                                  
#define TR_BLMODE                  "Licht Steuerung"                                 
#define TR_BLDELAY                "Licht aus nach"                                  
#define TR_SPLASHSCREEN                 "Startbild"                                       
#define TR_THROTTLEWARNING              "T-Warnung"                                       
#define TR_SWITCHWARNING                "S-Warnung"                                
#define TR_MEMORYWARNING                "Warnung Memory"                                  
#define TR_ALARMWARNING                 "Warnung Alarm"                                   
#define TR_TIMEZONE                     "Zeitzone"                                        
#define TR_RXCHANNELORD                 "Kanalanordnung"                                  
#define TR_SLAVE                        "Slave"                                           
#define TR_MODESRC                      "MODE\003% QUELLE"                                
#define TR_MULTIPLIER                   "Multiplikator"                                   
#define TR_CAL                          "Kal."                                            
#define TR_EEPROMV                      "EEPROM v"                                        
#define TR_VTRIM                        "Trim- +"                                         
#define TR_BG                           "BG:"                                             
#define TR_MENUTOSTART                  "[MENU] > Starten"                                
#define TR_SETMIDPOINT                  "SET MIDPOINT"                                    
#define TR_MOVESTICKSPOTS               "MOVE STICKS/POTS"                                
#define TR_RXBATT                       "Rx Batt:"                                        
#define TR_TXnRX                        "Tx:\0Rx:"                                        
#define OFS_RX                          4                                                 
#define TR_ACCEL                        "Acc:"                                            
#define TR_NODATA                       "NO DATA"                                         
#define TR_TM1TM2                       "TM1\015TM2"                                      
#define TR_THRTHP                       "THR\015TH%"                                      
#define TR_TOT                          "TOT"                                             
#define TR_TMR1LATMAXUS                 "Tmr1Lat max\003us"                               
#define STR_US (STR_TMR1LATMAXUS+12)
#define TR_TMR1LATMINUS                 "Tmr1Lat min\003us"                                                               
#define TR_TMR1JITTERUS                 "Tmr1 Jitter\003us"                                                               
#if defined(PCBSKY9X)
#define TR_TMAINMAXMS          "Tmain max\004ms"
#else
#define TR_TMAINMAXMS          "Tmain max\005ms"
#endif
#define TR_T10MSUS                      "T10ms\007us"                                                                     
#define TR_FREESTACKMINB                "Free Stack\004b"                                                                 
#define TR_MENUTORESET                  "[MENU] f\205r Reset"                                                             
#define TR_PPM                          "PPM"                                                                             
#define TR_CH                           "CH"                                                                              
#define TR_MODEL                        "MODELL"                                                                          
#define TR_FP                           "FP"                                                                              
#define TR_EEPROMLOWMEM                 "EEPROM low mem"                                                                  
#define TR_ALERT                        "WARNUNG"                                                                         
#define TR_PRESSANYKEYTOSKIP            "Taste dr\205cken"                                                                
#define TR_THROTTLENOTIDLE              "Schub nicht Null!"                                                                
#define TR_ALARMSDISABLED               "Alarme ausgeschaltet"                                                            
#define TR_PRESSANYKEY                  "\004Taste dr\205cken"                                                            
#define TR_BADEEPROMDATA                "Bad EEprom Data"                                                                 
#define TR_EEPROMFORMATTING             "EEPROM Formatting"                                                               
#define TR_EEPROMOVERFLOW               "EEPROM overflow"                                                                 
#define TR_MENURADIOSETUP               "RADIO SETUP"                                                                     
#define TR_MENUDATEANDTIME              "DATUM UND ZEIT"                                                                  
#define TR_MENUTRAINER                  "TRAINER"                                                                         
#define TR_MENUVERSION                  "VERSION"                                                                         
#define TR_MENUDIAG                     "DIAG"                                                                            
#define TR_MENUANA                      "ANALOG INPUTS"                                                                   
#define TR_MENUCALIBRATION              "KALIBRIERUNG"
#define TR_TRIMS2OFFSETS                "Trims => Offsets"                                                                
#define TR_MENUMODELSEL                 "MODELLE"                                                                         
#define TR_MENUSETUP                    "SETUP"                                                                           
#define TR_MENUFLIGHTPHASE              "FLUG PHASE"                                                                      
#define TR_MENUFLIGHTPHASES             "FLUG PHASEN"                                                                     
#define TR_MENUHELISETUP                "HELI SETUP"                                                                      
#define TR_MENUDREXPO                   "DR/EXPO" // TODO flash saving this string is 2 times here                        
#define TR_MENULIMITS                   "LIMITS"                                                                          
#define TR_MENUCURVES                   "KURVEN"                                                                          
#define TR_MENUCURVE                    "KURVE"
#define TR_MENUCUSTOMSWITCH             "CUSTOM SWITCH"
#define TR_MENUCUSTOMSWITCHES           "CUSTOM SWITCHES"                                                                 
#define TR_MENUFUNCSWITCHES             "FUNC SWITCHES"                                                                   
#define TR_MENUTELEMETRY                "TELEMETRY"                                                                       
#define TR_MENUTEMPLATES                "TEMPLATES"                                                                       
#define TR_MENUSTAT                     "STATS"                                                                           
#define TR_MENUDEBUG                    "DEBUG"                                                                           
#define TR_RXNUM                        "RxNum"                                                                           
#define TR_SYNCMENU                     "Sync [MENU]"                                                                     
#define TR_BACK                         "Zur\205ck"                                                                       
#define TR_MINLIMIT                     "Min Limit"                                                                       
#define STR_LIMIT                       (STR_MINLIMIT+4)                                                                  
#define TR_MAXLIMIT                     "Max Limit"                                                                       
#define TR_MINRSSI                      "Min Rssi"                                                                        
#define TR_LATITUDE                     "Latitude"                                                                        
#define TR_LONGITUDE                    "Longitude"                                                                       
#define TR_GPSCOORD                     "Gps Koord."                                                                      
#define TR_VARIO                        "Vario"                                                                           
#define TR_SHUTDOWN                     "SHUTTING DOWN"                                                                   
#define TR_BATT_CALIB                   "Batt. Kalib."                                                                    
#define TR_CURRENT_CALIB                "Strom Kalib."
#define TR_VOLTAGE                      "Voltage"
#define TR_CURRENT                      "Strom"                                                                           
#define TR_SELECT_MODEL                 "Modell W\201hlen"                                                                
#define TR_CREATE_MODEL                 "Modell Neu"                                                                      
#define TR_BACKUP_MODEL                 "Modell Backup"                                                                   
#define TR_DELETE_MODEL                 "Modell L\203schen" // TODO merged into DELETEMODEL?                              
#define TR_RESTORE_MODEL                "Modell Restore"                                                                  
#define TR_SDCARD_ERROR                 "SDcard Error"                                                                    
#define TR_NO_SDCARD                    "Keine SDcard"
#define TR_INCOMPATIBLE                 "Incompatible"
#define TR_WARNING                      "WARNUNG"
#define TR_EEPROMWARN                   "EEPROM"
#define TR_THROTTLEWARN                 "GAS"
#define TR_ALARMSWARN                   "ALARM"
#define TR_SWITCHWARN                   "SCHALTER"
#define TR_INVERT_THR                   "Invert Thr?"
#define TR_SPEAKER_VOLUME      "Speaker Volume"
#define TR_OPTREX_DISPLAY      "Optrex Display"
#define TR_BRIGHTNESS          "Brightness"
#define TR_CPU_TEMP            "CPU temp.\007>"
#define TR_CPU_CURRENT         "Current\011>"
#define TR_CPU_MAH             "Consumpt."
#define TR_COPROC              "CoProc."
#define TR_COPROC_TEMP         "MB temp. \007>"
#define TR_CAPAWARNING         "Capacity Warning"
#define TR_TEMPWARNING         "Temp Warning"
#define TR_FUNC                "Func"
#define TR_V1                  "V1"
#define TR_V2                  "V2"
#define TR_DURATION            "Duration"
#define TR_DELAY               "Delay"
#define TR_SD_CARD             "SD Card"
#define TR_SDHC_CARD           "SD-HC Card"
#define TR_NO_SOUNDS_ON_SD     "No Sounds on SD"
#define TR_NO_MODELS_ON_SD     "No Models on SD"
#define TR_PLAY_FILE           "Play"
#define TR_DELETE_FILE         "Delete"
#define TR_COPY_FILE           "Copy"
#define TR_RENAME_FILE         "Rename"
#define TR_REMOVED             " removed"
#define TR_SD_INFO             "Information"
#define TR_SD_FORMAT           "Format"
#define TR_NA                  "N/A"
