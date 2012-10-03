//#include <ioavr.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include "stdint.h"
//#include <inavr.h>
#include "Common_Define.h"
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "USI_TWI_Slave.h"

#define sbi(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbi(port,bit)  (port &= ~(1<<bit))  //clear bit in port
#define SLEEP_MODE_PWR_SAVE (_BV(SM0) | _BV(SM1))
#define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
#define NOINLINE __attribute__ ((noinline))

// Version number, sent in first byte of response
#define VERSION				 0x03

/*****************************************************************************/
// USI_TWI write states.
#define USI_TWI_WRITE_ADDR_HI_BYTE              (0x00)
#define USI_TWI_WRITE_ADDR_LO_BYTE              (0x01)
#define USI_TWI_WRITE_DATA_BYTE                 (0x02)

// USI_TWI overflow states.
#define USI_TWI_OVERFLOW_STATE_NONE             (0x00)
#define USI_TWI_OVERFLOW_STATE_ACK_PR_RX        (0x01)
#define USI_TWI_OVERFLOW_STATE_DATA_RX          (0x02)
#define USI_TWI_OVERFLOW_STATE_ACK_PR_TX        (0x03)
#define USI_TWI_OVERFLOW_STATE_PR_ACK_TX        (0x04)
#define USI_TWI_OVERFLOW_STATE_DATA_TX          (0x05)

#define DATA_SIZE		16		// Receive data buffer size

#define TEMPERATURE				0b01001011    

// Signature row addresses
#define TSOFFSET		5
#define TSGAIN			7

// Bits in AdcControl
#define	ADC_RUNNING			0x80
#define ADC_COUNT_MASK	0x0F

uint8_t AdcControl ;
uint16_t AdcTotal ;
int8_t	T_offset ;
uint8_t T_gain ;
int16_t T_mult ;

// USI_TWI state values.
static uint8_t USI_TWI_SLAVE_Write_State;
static uint8_t USI_TWI_SLAVE_Overflow_State;
static uint8_t Command_Flag;

// TWI flag values.
// static uint8_t USI_TWI_SLAVE_Address_Update;

// Slave Each page address values.
//static uint16_t USI_TWI_SLAVE_PAGE_Address;

uint8_t DataBuffer[DATA_SIZE];
static uint8_t *bufferPtr ;
static uint8_t Value;
uint8_t Tx_buffer[22] ;
int8_t Temperature;
static uint8_t Tx_index ;

typedef struct
{ 
	unsigned char second;   //enter the current time, date, month, and year
	unsigned char minute;
	unsigned char hour;                                     
	unsigned char date;       
	unsigned char month;
	unsigned int year;      
 } t_time ;

t_time Time ;

uint8_t ActivityFlag ;

void main (void) __attribute__((noreturn)) ;
char not_leap(void) ;      //check for leap year
void updateTime(uint8_t amount) ;
void set_clock( uint8_t speed ) ;
void checkPowerSave( void ) ;
static void Disable_WatchDogTimer(void) ;
static void Init_WatchDogTimer(void) ;
void enableAdc( void ) ;
void disableAdc( void ) ;


char not_leap(void)      //check for leap year
{
	t_time *p = &Time ;
	FORCE_INDIRECT(p) ;
  if (!(p->year%100))
    return (char)(p->year%400);
  else
    return (char)(p->year%4);
}         

void updateTime(uint8_t amount)
{ 
	t_time *p = &Time ;
	FORCE_INDIRECT(p) ;
    
  if (++p->second >= 60)        //keep track of time, date, month, and year
  {
    p->second -= 60 ;
    if (++p->minute >= 60) 
    {
      p->minute=0;
      if (++p->hour >= 24)
      {
        p->hour=0;
        if (++p->date >= 32)
        {
          p->month++;
          p->date=1;
        }
        else if (p->date == 31) 
        {                    
          if ((p->month==4) || (p->month==6) || (p->month==9) || (p->month==11)) 
          {
            p->month++;
            p->date=1;
          }
        }
        else if (p->date==30)
        {
          if(p->month==2)
          {
            p->month++;
            p->date=1;
          }
        }              
        else if (p->date==29) 
        {
          if((p->month==2) && (not_leap()))
          {
            p->month++;
            p->date=1;
          }                
        }                          
        if (p->month >= 13)
        {
          p->month=1;
          p->year++;
        }
      }
    }
  }  
}

ISR(TIMER0_OVF_vect)
{
	TCCR0B = 5 ;		// Divide/128
  while((ASSR & 0b00011011) != 0x00);/*Wait until TCNT0, OCR0A, TCCR0A and TCCR0B updated*/
	updateTime(1) ;
}

void checkPowerSave()
{
	if ( PINB & 0x05 )
	{
		ActivityFlag = 0 ;
		return ;	// I2C line(s) are high, power is ON
	}
	if ( ActivityFlag < 3 )
	{
		return ;
	}

// The AVR toolchain includes a header file, sleep.h, which has functions for setting
//sleep mode, disabling BOD, and putting the device into sleep. These functions are
//called:
//.. set_sleep_mode() – selects which sleep mode to use when the sleep
//instruction is executed
//.. sleep_enable() – enables sleep modes
//.. sleep_bod_disable() – disables BOD while sleeping
//.. sleep_cpu() – executes the sleep instruction
	
	CLKPR = 0x80 ;		// Do inline for quick response
	CLKPR = 0 ;				// Full speed 8.0MHz
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	TIMSK0 |= (1 << TOIE0) ;
	disableAdc() ;
  PRR = 0x3B ;		// Power off USI
	Disable_WatchDogTimer() ;
	set_clock( 2 ) ;		// Slow to 2.0 MHz
	while ( ( PINB & 0x05 ) == 0 )		// I2C lines are low
  {
  	sleep_enable();
  	sleep_bod_disable();
  	sei();
  	sleep_cpu();
  	sleep_disable();
		cli() ;
	 	wdt_reset() ;
  }
  PRR = 0x39 ;		// USI re-powered
  Init_WatchDogTimer() ;
  USI_TWI_SLAVE_Init();
	enableAdc() ;
	TIMSK0 &= ~(1 << TOIE0) ;
  USISR = (1<<USISIF) | (1<<USIOIF) | (1<<USIPF) ;        // Clear interrupt flags.
	set_clock( 5 ) ;		// Slow to 0.25MHz
	ActivityFlag = 0 ;
}


/******************************************************************************/

/* Local variables   */
//static uint8_t TWI_RxBuf[TWI_RX_BUFFER_SIZE];
//static volatile uint8_t TWI_RxHead;
//static volatile uint8_t TWI_RxTail;

//static uint8_t TWI_TxBuf[TWI_TX_BUFFER_SIZE];
//static volatile uint8_t TWI_TxHead;
//static volatile uint8_t TWI_TxTail;

#define USI_TWI_SlaveAddress 0x35

//********** USI_TWI functions **********//
static void USI_TWI_SLAVE_Init()
{
  
//  USI_TWI_SlaveAddress = TWI_OwnAddress;
  
  DDR_USI  &= ~(1<<PORT_USI_SDA);      // Set SDA as input
  PORT_USI &=  ~(1<<PORT_USI_SDA);    // Set SDA high
  
  DDR_USI  |=  (1<<PORT_USI_SCL);      // Set SCL as output
  PORT_USI |=  (1<<PORT_USI_SCL);      // Set SCL high
   
  
  USICR    =  /*(0<<USISIE)|(0<<USIOIE)|*/                         // Disable Start Condition Interrupt. Disable Overflow Interrupt.
               (1<<USIWM1)|(1<<USIWM0)|                        // Set USI in Two-wire mode. No USI Counter overflow prior
                                                               // to first Start Condition (potentail failure)
               (1<<USICS1)|(0<<USICS0)|(0<<USICLK)|            // Shift Register Clock Source = External, positive edge
              (0<<USITC);
   
  // Clear the interrupt flags and reset the counter.   
  USISR = (1<<USISIF) | (1<<USIOIF) | (1<<USIPF) |        // Clear interrupt flags.
          (1<<USIDC) |(0x0<<USICNT0);                     // USI to sample 8 bits or 16 edge toggles.
  
   // USI TWI Start condition interrupt enable.
//    USICR |= (1<<USISIE);			// Not needed, all are polled
 }

 // the below function is added by 
/*****************************************************************************/ 
static void USI_TWI_SLAVE_Abort()
{
  USICR    =  /*(0<<USISIE)|(0<<USIOIE)|*/                         // Disable Start Condition Interrupt. Disable Overflow Interrupt.
               (0<<USIWM1)|(0<<USIWM0)|                            // reset USI in Two-wire mode. No USI Counter overflow prior
                                                                 // to first Start Condition (potentail failure)
               (0<<USICS1)|(0<<USICS0)|(0<<USICLK)|                // Shift Register Clock Source = internal, positive edge
              (0<<USITC); 
  
  
  // Reset SCL and SDA lines of TWI Communication interface
   DDR_USI  &= ~(1<<PORT_USI_SDA);      
  PORT_USI &=  ~(1<<PORT_USI_SDA);    // Set SDA low
//		 	PORTA &= ~8 ;
  
  DDR_USI  |=  ~(1<<PORT_USI_SCL);     
  PORT_USI |=  ~(1<<PORT_USI_SCL);      // Set SCL low
  
  // Clear the USI DATA register.
  USIDR = 0x00;
  
  // Clear the interrupt flags and reset the counter.   
  USISR = (1<<USISIF) | (1<<USIOIF) | (1<<USIPF) |        // Clear interrupt flags.
          (1<<USIDC) |(0x0<<USICNT0);                     // USI to sample 8 bits or 16 edge toggles.
  
}
 

static void USI_TWI_SLAVE_ReadAndProcessPacket ()
{
	set_clock( 5 ) ;		// Slow to 0.25MHz
	while(1)
	{
   // Check for USI TWI start condition.
	  if (USISR & (1<<USISIF))
  	{
			CLKPR = 0x80 ;		// Do inline for quick response
			CLKPR = 0 ;				// Full speed 8.0MHz
      // Process the USI TWI start condition.
      USI_TWI_SLAVE_Process_Start_Condition();

  	}
  
   // Check for USI_TWI_SLAVE overflow condition.
  	if (USISR & (1<<USIOIF))
	  {
	 		ActivityFlag = 0 ;
      // Handle the TWI overflow condition.
      USI_TWI_SLAVE_Process_Overflow_Condition();
      
     /*  as this part is already taken care...
      // Should we update the twi address?
      if (USI_TWI_SLAVE_Address_Update)
      {
          // Mark the bootloader as active.
          bootloader_active = 1;
          
          // Check for the special address to exit the bootloader.
          if (USI_TWI_SLAVE_PAGE_Address != 0xffff)
          {
            // Set the twi address.  This will load the corresponding page from
            // flash into the programming buffer for reading and writing.
            // prog_buffer_set_address(USI_TWI_SLAVE_PAGE_Address);
          }
          
          // Reset the flag.
          USI_TWI_SLAVE_Address_Update = 0;
      }
      
      .....Till here.........*/
      
	  }
   
   // Check for TWI stop condition.
	  if (USISR & (1<<USIPF))
  	{
       // Clear the stop condition flag.
        USISR = (1<<USIPF);
        // Check for the special address to exit the bootloader.
        if ( Run_boot )
        {
          // Set the flag to have the bootloader eixt.
          break ;
        }
        else
        {
			    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_NONE ;
					Command_Flag = 0 ;
        }
			set_clock( 5 ) ;		// Slow to 0.25MHz
		}
		wdt_reset() ;

		if ( TIFR0 & 1 )
		{
			CLKPR = 0x80 ;		// Do inline for quick response
			CLKPR = 0 ;				// Full speed 8.0MHz
	 	
			TIFR0 = 1 ;		// CLEAR flag (write 1)
			updateTime(1) ;
			t_time *p = &Time ;
			uint8_t *q = &Tx_buffer[1] ;
//		FORCE_INDIRECT(p) ;

			*q++ = p->second ;
			*q++ = p->minute ;
			*q++ = p->hour ;
			*q++ = p->date ;
			*q++ = p->month ;
			*q++ = p->year ;
			*q++ = p->year >> 8 ;
			*q++ = Temperature ;
			*q++ = T_offset ;
			*q++ = T_gain ;
//		*q++ = MCUSR ;

			if ( ActivityFlag < 255 )
			{
				ActivityFlag += 1 ;
			}
			set_clock( 5 ) ;		// Slow to 0.25MHz
		}

		checkPowerSave() ;

		if ( ( AdcControl & ADC_RUNNING ) == 0 )
		{
		  ADMUX = TEMPERATURE ;       // Internal 1.1V REF and ADCx
		  ADCSRA |= (1<<ADSC);        // do single conversion
			AdcControl |= ADC_RUNNING ;
		}
		else
		{
			if ( ( ADCSRA & (1<<ADSC) ) == 0 )
			{
				AdcControl &= ~ADC_RUNNING ;
				AdcControl += 1 ;
				if ( ( AdcControl & ADC_COUNT_MASK ) > 1 )
				{ // Skip first conversion
					AdcTotal += ADCW ;				
				}
				if ( ( AdcControl & ADC_COUNT_MASK ) >= 9 )
				{
					AdcTotal += 4 ;
					AdcTotal >>= 3 ;
					{
						int16_t temp ;

						temp = AdcTotal ;
						temp -= 298 ;
						temp += T_offset ;
						temp *= T_mult ;
						temp >>= 8 ;
						temp += 25 ;
						Temperature = temp ;
					}
					AdcControl = 0 ;
					AdcTotal = 0 ;


//    			if (AdcTotal < 0x00f6)
//					{
//    			  Temperature=-40;
//    			}
//					else if (AdcTotal < 0x0144)
//					{
//    			  Temperature=((AdcTotal*78)/65)-40;
//    			}
//					else
//					{
//    			  Temperature=((((AdcTotal-324)*600)/74)+250)/10;
//    			}  
				}
			}
		}
  } // while
//  Disable_WatchDogTimer(); // After Reset the WDT state does not change
//  void (*FuncPtr) (void) = (void (*)(void)) (0x0080);	// Set up function pointer to address 0x0080
//  FuncPtr ();
	TCCR0B = 0 ;
  USICR = 0 ;
  USISR = 0 ;
  PRR = 0 ;
	CLKPR = 0x80 ;		// Do inline for quick response
	CLKPR = 0 ;				// Full speed 8.0MHz
	((void (*)(void)) (0))() ;
}


/******************************************************************************/
//Description: 
// Process the USI TWI start condition.  This is called when the TWI master initiates
// communication with a USI TWI slave by asserting the TWI start condition.
/******************************************************************************/
static void USI_TWI_SLAVE_Process_Start_Condition(void)
{    
    // Wait until the "Start Condition" is complete when SCL goes low. If we fail to wait
    // for SCL to go low we may miscount the number of clocks pulses for the data because
    // the transition of SCL could be mistaken as one of the data clock pulses.
    while ((PIN_USI & (1<<PORT_USI_SCL)));
    // Reset the overflow state.
    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_NONE;
    // Clear the interrupt flags and reset the counter.
    USISR = (1<<USISIF) | (1<<USIOIF) | (1<<USIPF) |    // Clear interrupt flags.
            (0x00<<USICNT0);                            // USI to sample 8 bits or 16 edge toggles.
    
    // Update the interrupt enable, wire mode and clock settings.
    USICR = /*(1<<USISIE) | (1<<USIOIE) |*/                 // Enable Overflow and Start Condition Interrupt.
            (1<<USIWM1) | (1<<USIWM0) |                 // Maintain USI in two-wire mode with clock stretching.
            (1<<USICS1) | (0<<USICS0) | (0<<USICLK) |   // Shift Register Clock Source = External, positive edge
            (0<<USITC);                                 // No toggle of clock pin.
}

/******************************************************************************/


/*******************************************************************************
Description: 
Processes the USI_TWI_SLAVE overflow condition.  
This is called when the USI TWI 4-bit counterboverflows indicating the 
TWI master has clocked in/out a databyte or a singleback/nack byte following a 
databyte transfer.

*******************************************************************************/
void USI_TWI_SLAVE_Process_Overflow_Condition(void)
{    
    // Buffer the USI data.
    uint8_t Usi_Data = USIDR;
  
  // Handle the interrupt based on the overflow state.
  switch (USI_TWI_SLAVE_Overflow_State)
  {
    /***********************************************************************/
    // Handle the first byte transmitted from master -- the slave address.
    case USI_TWI_SLAVE_OVERFLOW_STATE_NONE:
      
      // Are we receiving our address?
      if ((Usi_Data >> 1) == USI_TWI_SlaveAddress)
      {
        // Yes. Are we to send or receive data?
        if((Usi_Data & 0x01) == 0x01)
        {
            //USI TWI Slave has to transmit the data byte
            USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_TX;
						Tx_buffer[0] = VERSION ;				// Version number
						Tx_index = 0 ;
        }
        else
        {
            //USI TWI Slave has to Receive the data byte
            USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_RX;
        }
        
        // Reset the write state.
       //.. added the below statement at Command_flag=1
        //USI_TWI_SLAVE_Write_State = USI_TWI_SLAVE_WRITE_ADDR_HI_BYTE;
        
        // Set SDA for output.
        PORT_USI |= (1<<PORT_USI_SDA);
        DDR_USI |= (1<<PORT_USI_SDA);
        
        // Load data for ACK.
        USIDR = 0;
        
        // Reload counter for ACK -- two clock transitions.
        USISR = 0x0E;
        
      }
      else
      {
        // No. Reset USI to detect start condition.  Update the interrupt enable, 
        // wire mode and clock settings.  Note: At this time the wire mode must
        // not be set to hold the SCL line low when the counter overflows.  
        // Otherwise, this TWI slave will interfere with other TWI slaves.
        USICR = /*(1<<USISIE) | (0<<USIOIE) |*/                 // Enable Start Condition Interrupt. Disable overflow.
          (1<<USIWM1) | (0<<USIWM0) |                 			// Maintain USI in two-wire mode without clock stretching.
            (1<<USICS1) | (0<<USICS0) | (0<<USICLK) |   		// Shift Register Clock Source = External, positive edge
              (0<<USITC);                                 	// No toggle of clock pin.
      }
      
      break;
       /***********************************************************************/   
      // Ack sent to master so prepare to receive more data.
	case USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_RX:
    
      if(Command_Flag == 0)
      {
         // Update our state.
         USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_CMD_RX;
      }
      else if(Command_Flag == 1)
      {
         // Update our state.
         USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_RX;
      }
//      else if(Command_Flag == 2)
//      {
//        // Update our state.
//         USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_AVERSION;
//      }
//       else if(Command_Flag == 3)
//      {
//        // Update our state.
//         USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_BVERSION;
//      }
//       else if(Command_Flag == 4)
//      {
//        // Update our state.
//         USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_GETERRCONDN;
//      }
       
    
     // Set SDA for input
     DDR_USI &= ~(1<<PORT_USI_SDA);
     PORT_USI &= ~(1<<PORT_USI_SDA);
    
	break;
    /**************************************************************************/
    
    
  case USI_TWI_SLAVE_OVERFLOW_STATE_CMD_RX:
       
     // Update our state
    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_NONE;
   
      // Check the command received type.
      if(Usi_Data == TWI_CMD_REBOOT)
      {
          // reset the controller ... 
					USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_RX ;
          // set this flag to receive data
          Command_Flag = 0 ;
					Run_boot = 1 ;
      }
      else if(Usi_Data == TWI_CMD_WRITE_DATA)
      {
          // set this flag to receive data
          Command_Flag =1;
//					ReceiveType = TWI_CMD_SETDATETIME ;
					USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_RX ;
		      USI_TWI_SLAVE_Write_State = USI_TWI_SLAVE_WRITE_DATA_BYTE;
		      bufferPtr = DataBuffer ;
					Value = 0 ;
      }
//      else if(Usi_Data == TWI_CMD_BVERSION)
//      {
//          // set this flag to receive data
//          Command_Flag =3; 
        
//      }
//      else if(Usi_Data == TWI_CMD_ERASEFLASH)
//      {
//          // erase the flash here itself ....... 
        
//          // set this flag to receive data
//          Command_Flag =0; 
        
//      }
//      else if(Usi_Data == TWI_CMD_GETERRCONDN)
//      {
//          // set this flag to receive data
//          Command_Flag =4; 
        
//      }
      else
      {
          // set this flag to receive data
          Command_Flag =0; 
        
          USI_TWI_SLAVE_Abort();
      }
    // Set SDA for output.
    PORT_USI |= (1<<PORT_USI_SDA);
    DDR_USI |= (1<<PORT_USI_SDA);
    
    // Load data for ACK.
    USIDR = 0;
    
    // Reload counter for ACK -- two clock transitions.
    USISR = 0x0E;
  break;
    
    // Data received from master so prepare to send ACK.
  case USI_TWI_SLAVE_OVERFLOW_STATE_DATA_RX:
    
    // Update our state.
    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_RX;
    
    // Always make sure the Command_Flag is 1
    Command_Flag = 1;
    
    // Check the TWI write state to determine what type of byte we received.
//    if (USI_TWI_SLAVE_Write_State == USI_TWI_SLAVE_WRITE_ADDR_HI_BYTE)
//    {
//      // Set the twi address high byte.
//      USI_TWI_SLAVE_PAGE_Address = Usi_Data;
      
//      // Set the next state.
//      USI_TWI_SLAVE_Write_State = USI_TWI_SLAVE_WRITE_ADDR_LO_BYTE;
//    }
//    else if (USI_TWI_SLAVE_Write_State == USI_TWI_SLAVE_WRITE_ADDR_LO_BYTE)
//    {
//      // Set the address low byte.
//      USI_TWI_SLAVE_PAGE_Address = (USI_TWI_SLAVE_PAGE_Address << 8) | Usi_Data;
      
//      // Set the programming address.
////      USI_TWI_SLAVE_Address_Update = 1;
//      Value = 0;
//      bufferPtr = pageBuffer ;
      
//      // Set the next state.
//      USI_TWI_SLAVE_Write_State = USI_TWI_SLAVE_WRITE_DATA_BYTE;
//    }
//    else
    {
        // Write the data to the buffer.
        //USI_TWI_SLAVE_prog_buffer_set_byte(Usi_Data);
      
        *bufferPtr = Usi_Data;

        // Increment the byte address within the page.
        //++prog_byte_address;
      
        ++bufferPtr;
        ++Value;
        // Check the byte address for wrapping. //  check later 

				if ( DataBuffer[0] == TWI_SUBCMD_SETDATETIME )
				{
        	if (Value >= 8 ) // Size of time structure+1
        	{
						if ( Value == 8 )
						{
							// Set the date and time
							t_time *p = &Time ;
							FORCE_INDIRECT(p) ;

							p->second = DataBuffer[1] ;
							p->minute = DataBuffer[2] ;
							p->hour = DataBuffer[3] ;
							p->date= DataBuffer[4] ;
							p->month = DataBuffer[5] ;
							p->year = DataBuffer[6] + ( DataBuffer[7] << 8 ) ;
						}
        	  Value = 9 ;
        	  bufferPtr = &DataBuffer[DATA_SIZE-1] ;
        	}
				}
				if ( Value > DATA_SIZE )
				{
          Value = DATA_SIZE ;
          bufferPtr = &DataBuffer[DATA_SIZE-1] ;	// Overwrite last byte
				}
    }
    
		// Set SDA for output.
    PORT_USI |= (1<<PORT_USI_SDA);
    DDR_USI |= (1<<PORT_USI_SDA);
//			 	PORTA |= 8 ;
    
    // Load data for ACK.
    USIDR = 0;
    
    // Reload counter for ACK -- two clock transitions.
    USISR = 0x0E;
    
    break;
     /***********************************************************************/
    
  case USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_AVERSION:
    
     // Update our state.
    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_PR_ACK_TX;
    
    // Set SDA for input.
    DDR_USI &= ~(1<<PORT_USI_SDA);
//		 	PORTA &= ~8 ;
    PORT_USI &= ~(1<<PORT_USI_SDA);
    
    // Reload counter for ACK -- two clock transitions.
    USISR = 0x0E;
    
  break;
    
    /***********************************************************************/
     case USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_BVERSION:
    
    break;
   /***********************************************************************/ 
     case USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_GETERRCONDN:
    
    break;
  /***********************************************************************/  
    // ACK received from master.  Reset USI state if NACK received.
  case USI_TWI_SLAVE_OVERFLOW_STATE_PR_ACK_TX:
    
    // Check the lowest bit for NACK?  If set, the master does not want more data.
    if (Usi_Data & 0x01)
    {
      // Update our state.
      USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_NONE;
      
      // Reset USI to detect start condition. Update the interrupt enable,
      // wire mode and clock settings. Note: At this time the wire mode must
      // not be set to hold the SCL line low when the counter overflows.
      // Otherwise, this TWI slave will interfere with other TWI slaves.
      USICR = /*(1<<USISIE) | (0<<USIOIE) |*/                 // Enable Start Condition Interrupt. Disable overflow.
        (1<<USIWM1) | (0<<USIWM0) |                 // Maintain USI in two-wire mode without clock stretching.
          (1<<USICS1) | (0<<USICS0) | (0<<USICLK) |   // Shift Register Clock Source = External, positive edge
            (0<<USITC);                                 // No toggle of clock pin.
      
      
      // Clear the overflow interrupt flag and release the hold on SCL.
      USISR |= (1<<USIOIF);
      
      return;
    }
    
    
    
     /***********************************************************************/   
    // Handle sending a byte of data.
  case USI_TWI_SLAVE_OVERFLOW_STATE_ACK_PR_TX:
    
    // Update our state.
   // USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX;
     USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX_AVERSION;
    
    // Set SDA for output.
    PORT_USI |= (1<<PORT_USI_SDA);
    DDR_USI |= (1<<PORT_USI_SDA);
//			 	PORTA |= 8 ;
    
    // Get the data to send.
    // USIDR = prog_buffer_get_byte();
    
		USIDR = Tx_buffer[Tx_index++] ;		// Send this for testing
    
    break;
   /***********************************************************************/
    
    // Data sent to to master so prepare to receive ack.
  case USI_TWI_SLAVE_OVERFLOW_STATE_DATA_TX:
    
    // Update our state.
    USI_TWI_SLAVE_Overflow_State = USI_TWI_SLAVE_OVERFLOW_STATE_PR_ACK_TX;
    
    // Set SDA for input.
    DDR_USI &= ~(1<<PORT_USI_SDA);
//		 	PORTA &= ~8 ;
    PORT_USI &= ~(1<<PORT_USI_SDA);
    
    // Reload counter for ACK -- two clock transitions.
    USISR = 0x0E;
    
    break;
   /***********************************************************************/
  }
  // Clear the overflow interrupt flag and release the hold on SCL.
  USISR |= (1<<USIOIF);
}

uint8_t state = 0;
// __no_init uint8_t pageBuffer[PAGE_SIZE];
//uint16_t time_lapse_sec = 0; 
#define gtimeout WDT_TIMEOUT_8s
// uint8_t volatile gtimeout;
//uint8_t statusCode = 0;

/***********************************************************************/

static void Disable_WatchDogTimer (void)
{
/*  
     Timed sequence
     Interrupts are already disabled else additional code may go in to avoid
     the servicing of interrupt in between the timed sequence
*/
	wdt_reset();
	WDTCR = (1 << WDCE) | ( 1 << WDE ) ;
	MCUSR = 0 ;
	WDTCR = 0 ;

}

/***********************************************************************/

static void Init_WatchDogTimer (void)
{
/*
	Timed sequence to initialize watchdog timer
	for the given mode set in gtimeout variable
	Cross calls during maximum optimization can
	cause more than 4 cycles delay between change
	enable for WDT and setting values
*/
	wdt_reset();
	WDTCR = (1 << WDCE) | ( 1 << WDE ) ;
	WDTCR = ( 1 << WDE ) | gtimeout ;
}


/***********************************************************************/
NOINLINE void set_clock( uint8_t speed )
{
	volatile uint8_t *ptr ;
	ptr = &CLKPR ;
	*ptr = 0x80 ;
	*ptr = speed ;
}

/***********************************************************************/
// Main Starts from here
void main (void)
{
  
  USI_TWI_SLAVE_Init();
           //__enable_interrupt();
    
  Init_WatchDogTimer() ;

	 /*RTC 32.768kHz, div 128 */
	{
    PRR = 0x39 ;
		TCCR0A = 0 ;
		TCCR0B = 5 ;		// Divide/128
		ASSR = 0x20 ;		// Async mode
    while((ASSR & 0b00011011) != 0x00);/*Wait until TCNT0, OCR0A, TCCR0A and TCCR0B updated*/
    TIFR0 = 0x01;/*Clear T0 int flag*/

	}
	t_time *p = &Time ;
	FORCE_INDIRECT(p) ;

	p->second = 5 ;
	p->minute = 6 ;
	p->hour = 16 ;
	p->date= 23 ;
	p->month = 9 ;
	p->year = 2012 ;

	bufferPtr = DataBuffer ;

	T_offset = boot_signature_byte_get( TSOFFSET ) ;
	T_gain = boot_signature_byte_get( TSGAIN ) ;
	T_mult = 32768U / T_gain ;

	enableAdc() ;
	for(;;)
	{
		USI_TWI_SLAVE_ReadAndProcessPacket() ;
	}
}


// Enable ADC  
void enableAdc()
{
  cbi( PRR, PRADC ) ;		// Power the ADC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);	// set ADC prescaler to , 8MHz / 64 = 125kHz    
}

// Disable ADC  
void disableAdc()
{
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) ;	// set ADC prescaler to , 8MHz / 256 = 30kHz
  sbi( PRR, PRADC ) ;		// Power Down the ADC
}

