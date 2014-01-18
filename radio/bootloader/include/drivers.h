#ifndef drivers_h
#define drivers_h


extern volatile uint32_t Spi_complete ;

extern void putEvent( register uint8_t evt) ;

extern void per10ms( void ) ;
extern uint8_t getEvent( void ) ;
extern void pauseEvents(uint8_t event) ;
extern void killEvents(uint8_t event) ;
extern void init_spi( void ) ;
extern void end_spi( void ) ;


//------------------------------------------------------------------------------
/// Detect if SD card is connected
//------------------------------------------------------------------------------
#ifdef PCBSKY
#define CardIsConnected() ( (PIOB->PIO_PDSR & PIO_PB7) == 0 )
#endif

extern uint32_t Card_ID[4] ;
extern uint32_t Card_SCR[2] ;
extern uint32_t Card_CSD[4] ;
extern uint32_t Sd_128_resp[4] ;
extern uint32_t Sd_rca ;
//extern uint32_t Cmd_55_resp ;

extern uint32_t Card_state ;

extern uint32_t SD_SetBusWidth( uint32_t busWidth) ;
extern void SD_EnableHsMode( uint8_t hsEnable) ;
extern uint32_t SD_SetSpeed( uint32_t mciSpeed ) ;
extern void SD_Reset( uint8_t keepSettings) ;
extern uint32_t sd_cmd55( void ) ;
extern uint32_t sd_acmd41( void ) ;
extern uint32_t sd_cmd2( void ) ;
extern uint32_t sd_cmd3( void ) ;
extern uint32_t sd_cmd7( void ) ;
extern uint32_t sd_cmd9( void ) ;
extern uint32_t sd_cmd17( uint32_t address, uint32_t *presult ) ;
extern uint32_t sd_acmd6( void ) ;
extern uint32_t sd_acmd51( uint32_t *presult ) ;
extern void sd_poll_10mS( void ) ;
extern uint32_t sd_card_ready( void ) ;
extern uint32_t sd_read_block( uint32_t block_no, uint32_t *data ) ;

class Key
{
#define FILTERBITS      4
#define FFVAL          ((1<<FILTERBITS)-1)
#define KSTATE_OFF      0
#define KSTATE_RPTDELAY 95 // gruvin: longer dely before key repeating starts
  //#define KSTATE_SHORT   96
#define KSTATE_START   97
#define KSTATE_PAUSE   98
#define KSTATE_KILLED  99
  uint8_t m_vals:FILTERBITS;   // key debounce?  4 = 40ms
  uint8_t m_dblcnt:2;
  uint8_t m_cnt;
  uint8_t m_state;
public:
  void input(bool val, EnumKeys enuk);
  bool state()       { return m_vals==FFVAL;                }
  void pauseEvents() { m_state = KSTATE_PAUSE;  m_cnt   = 0;}
  void killEvents()  { m_state = KSTATE_KILLED; m_dblcnt=0; }
  uint8_t getDbl()   { return m_dblcnt;                     }
};

extern Key keys[NUM_KEYS] ;

#endif
