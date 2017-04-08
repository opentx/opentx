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

#include "opentx.h"

#define LG_BUF 14
#define NB_LONG_BUF 3
#define NB_SHORT_BUF 3
#define LONG_BUF(val)  (val)
#define SHORT_BUF(val)  (val+NB_LONG_BUF)
#define VALSTR(val)  (rbuf[val][0] ? rbuf[val] : val_unknown)
#define APSIZE (BSS | DBLSIZE)

uint8_t i;  								// working variable
uint8_t state;  				    		// currrent state
uint8_t rval, rpack;	    				// received items
uint8_t xval[NB_LONG_BUF+NB_SHORT_BUF];		// expected value
uint8_t xpack[NB_LONG_BUF+NB_SHORT_BUF];	// expected packet
uint8_t ibuf[NB_LONG_BUF];					// subscripts on long buffers values
char rbuf[NB_LONG_BUF][LG_BUF];				// long receive buffers
char sbuf[NB_SHORT_BUF];					// short receive buffers
const char val_unknown[] = "?";
int32_t home_alt, save_alt, rel_alt, prev_alt, lift_alt, max_alt, abs_alt;	// integer values for altitude computations
int32_t gpstimer=0;
int32_t gpstime;
uint8_t ggareceived;
uint8_t beep_on;
uint8_t show_timer;

/*    Received data
Data are received as packets, each packet is identified by a prefix of seven
characters ('$GPGGA,' or '$GPRMC,')and is ended by one star plus two bytes checksum.
The values are terminated by a comma.

$GPGGA - Global Positioning System Fix Data, Time, Position and fix related data fora GPS receiver.

                                                      11
        1         2       3 4        5 6 7  8   9  10 |  12 13  14   15
        |         |       | |        | | |  |   |   | |   | |   |    |
    GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>

Field Number:
  1) Universal Time Coordinated (UTC)
  2) Latitude
  3) N or S (North or South)
  4) Longitude
  5) E or W (East or West)
  6) GPS Quality Indicator,
     0 - fix not available,
     1 - GPS fix,
     2 - Differential GPS fix
  7) Number of satellites in view, 00 - 12
  8) Horizontal Dilution of precision
  9) Antenna Altitude above/below mean-sea-level (geoid)
 10) Units of antenna altitude, meters
 11) Geoidal separation, the difference between the WGS-84 earth
     ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level
     below ellipsoid
 12) Units of geoidal separation, meters
 13) Age of differential GPS data, time in seconds since last SC104
     type 1 or 9 update, null field when DGPS is not used
 14) Differential reference station ID, 0000-1023
 *
 15) Checksum
 CrLf


$GPRMC - Recommended Minimum Navigation Information
                                                            12
        1         2 3       4 5        6 7   8   9    10  11|
        |         | |       | |        | |   |   |    |   | |
    RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh<CR><LF>

 Field Number:
  1) UTC Time
  2) Status, V = Navigation receiver warning
  3) Latitude
  4) N or S
  5) Longitude
  6) E or W
  7) Speed over ground, knots
  8) Track made good, degrees true. = =  Course over ground (COG)
  9) Date, ddmmyy
 10) Magnetic Variation, degrees
 11) E or W
 12) Checksum
*/

// GGA record prefix
#define PACK_GGA 0x47		// "G"
#define PACK_GGA3 0x41		// "A"
// value occurence number in the GGA packet
#define TIM 1
#define LAT 2
#define NOS 3
#define LON 4
#define EOW 5
#define FIX 6
#define SAT 7
#define DIL 8
#define ALT 9
#define MTR 10
#define GEO 11
#define MET 12
#define AGE 13
#define DIF 14

// RMC record prefix
#define PACK_RMC 0x52		// "R"
#define PACK_RMC2 0x4D		// "M"
#define PACK_RMC3 0x43		// "C"
// value occurence number in the RMC packet
#define TIM 1
#define NRW 2
#define LT1 3
#define NSO 4
#define LN1 5
#define EWE 6
#define SOG 7
#define COG 8
#define DAT 9
#define MAG 10
#define EAW 11

// end of packet
#define PACK_END 0x2a			//  *
// end of value
#define VAL_END 0x2c			//  ,

// stateful machine

//   Since the packets are sent continuously, we need to synchronize on the
//   reception of the three chars prefixing a packet, whatever they are.

// states values
#define WAIT_PACKET     1
#define WAIT_PACK_GGA1  2
#define WAIT_PACK_GGA2  3
#define WAIT_PACK_GGA3  4
#define WAIT_PACK_RMC2  5
#define WAIT_PACK_RMC3  6
#define WAIT_VAL_END	7
#define READ_VALUE      8

void menuViewTelemetryNMEA1(event_t event);
void menuViewTelemetryNMEA2(event_t event);
void menuViewTelemetryNMEA3(event_t event);
void menuViewTelemetryNMEA4(event_t event);
void title(char x);
void initval(uint8_t num, uint8_t pack, uint8_t val);
int32_t binary (char *str);
int32_t bintime (char *str);

#ifndef SIMU
ISR (USART0_RX_vect)
{
    uint8_t rl;
    // uint8_t rh;                         //USART control and Status Register 0 B
    uint8_t iostat;                     //USART control and Status Register 0 A

    rl = UDR0;
    iostat = UCSR0A;                    //USART control and Status Register 0 A
    /*
   bit 	7		6		5		4	3		2		1		0
        RxC0	TxC0	UDRE0	FE0	DOR0	UPE0	U2X0	MPCM0

        RxC0: 	Receive complete
        TXC0: 	Transmit Complete
        UDRE0: 	USART Data Register Empty
        FE0:	Frame Error
        DOR0:	Data OverRun
        UPE0:	USART Parity Error
        U2X0:	Double Tx Speed
        MPCM0:	MultiProcessor Comms Mode
*/
    if (iostat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)))
    {
        rl = xpack[0] = xpack[1] = xval[0] = xval[1] = 0;
        initval (LONG_BUF(2), PACK_GGA, TIM);   // always get UTC time for timer
        state = WAIT_PACKET;			         // restart on error
    }
    // rh = UCSR0B;                       //USART control and Status Register 0 B
    /* bit 	7		6		5		4		3		2		1		0
        RxCIE0	TxCIE0	UDRIE0	RXEN0	TXEN0	UCSZ02	RXB80	TXB80

        RxCIE0: Receive complete int enable
        TXCIE0: Transmit Complete int enable
        UDRIE0: USART Data Register Empty int enable
        RXEN0:	Rx enable
        TXEN0:	Tx Enable
        UCSZ02:	Character Size bit 2
        RXB80:	Rx data bit 8
        TXB80:	Tx data bit 8
*/
    switch (state)
    {
    case WAIT_PACKET:
        switch (rl)
        {
        case PACK_GGA:					// found a new GGA packet  "G"
            state = WAIT_PACK_GGA2;		// wait for the 2nd char
            break;
        case PACK_RMC:					// found a new RMS packet  "R"
            state = WAIT_PACK_RMC2;		// wait for the 2nd char
            break;
        }
        break;

    case WAIT_PACK_GGA2:				// received 2nd char  "G"
        if (rl == PACK_GGA)
            state = WAIT_PACK_GGA3;			// wait for 3rd character "A"
        else
            state = WAIT_PACKET;		// restart if not "G"
        break;
    case WAIT_PACK_GGA3:				// received 3rd char	"A"
        if (rl == PACK_GGA3)				// found
        {
            state = WAIT_VAL_END;		// wait for ","
            rpack = PACK_GGA;
            rval = 1;				//clear the buffer
            for (i = 0; i < NB_LONG_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;

    case WAIT_PACK_RMC2:				// wait for 2nd char	"M"
        if (rl == PACK_RMC2)
            state = WAIT_PACK_RMC3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_RMC3:				// wait for 3rd char	"C"
        if (rl == PACK_RMC3)
        {
            state = WAIT_VAL_END;		// wait for ","
            rpack = PACK_RMC;
            rval = 1;
            for (i = 0; i < NB_LONG_BUF; i++)	// clear buffer
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;

    case WAIT_VAL_END:
        if (rl == VAL_END)			// "," nach "GGA" oder "RMC"
        {
            state = READ_VALUE;
            rval = 1;
            for (i = 0; i < NB_LONG_BUF; i++)	// clear buffer
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;


    case READ_VALUE:
        switch (rl)
        {
        case PACK_END:
            if (rpack == PACK_GGA)
                ggareceived = 1;
            state = WAIT_PACKET;		// packet completed, wait for the next packet
            break;
        case VAL_END:					// comma found, value completed
            rval++;						// and get next value
            break;
        default:						// store the char in the corresponding buffer
            for (i = 0; i < NB_LONG_BUF; i++)
            {							// is it the expected value in the expected packet ?
                if (rpack == xpack[i] && rval == xval[i] && ibuf[i] < LG_BUF - 1)
                {						// yes, store the char
                    rbuf[i] [ibuf[i]] = rl;
                    ibuf[i]++;
                    rbuf[i] [ibuf[i]] = 0;
                }
            }
            for (i = NB_LONG_BUF; i < NB_LONG_BUF+NB_SHORT_BUF; i++) {
                if (rpack == xpack[i]   // is this the expected short value in the expected packet ?
                        &&  rval == xval[i])
                    sbuf[i-NB_LONG_BUF] = rl;      // yes, store the char
            }
        }
        break;
    }
}
#endif

void NMEA_Init (void)
{
#ifndef SIMU
    DDRE  &= ~(1 << DDE0);              // set RXD0 pin as input
    PORTE &= ~(1 << PORTE0);            // disable pullup on RXD0 pin

    // switch (Telem_baud)
    // {
    // case 1:
#undef BAUD
#define BAUD 4800
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    //    break;
    // }

    UCSR0A &= ~(1 << U2X0);			// disable double speed operation
    // set 8N1
    UCSR0B = 0|(0<< RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(0<<RXEN0)|(0<<TXEN0)|(0<<UCSZ02);
    UCSR0C = 0|(1 << UCSZ01) | (1 << UCSZ00);
    while ( UCSR0A & (1 << RXC0) )
        UDR0;							// flush receive buffer
#endif
    home_alt = rel_alt = ggareceived =0;
    gpstimer = -1;
    beep_on=1;
}

// TX Capabilities are not required for NMEA
// void NMEA_DisableTXD (void)
// {
//    UCSR0B &= ~(1 << TXEN0);            // disable TX
// }

// void NMEA_EnableTXD (void)
// {
//    UCSR0B |=  (1 << TXEN0);            // enable TX
// }

void NMEA_DisableRXD (void)
{
    UCSR0B &= ~(1 << RXEN0);            // disable RX
    UCSR0B &= ~(1 << RXCIE0);           // disable Interrupt
}

void NMEA_EnableRXD (void)
{
    for (i = 0; i < NB_LONG_BUF; i++)
    {
        ibuf[i] = 0;
        rbuf[i][0] = 0;
        xpack[i] = 0;
        xval[i] = 0;
    }
    initval (LONG_BUF(2), PACK_GGA, TIM);	// always get UTC time for timer
    state = WAIT_PACKET;					// wait for the next packet
    UCSR0B |=  (1 << RXEN0);				// enable RX
    UCSR0B |=  (1 << RXCIE0);				// enable Interrupt
}

void menuViewTelemetryNMEA(event_t event)
{
    menuViewTelemetryNMEA1(event);
}

// Start of NMEA menus 1-4 <<<<<<<<<<<<<<<<<<<<<<<<<<<

void menuViewTelemetryNMEA1(event_t event)
{
    switch(event)						// new event received, branch accordingly
    {
    case EVT_KEY_BREAK(KEY_LEFT):
        chainMenu(menuViewTelemetryNMEA4);
        break;
    case EVT_KEY_BREAK(KEY_RIGHT):
        chainMenu(menuViewTelemetryNMEA2);
        break;
    case EVT_KEY_LONG(KEY_UP):
        NMEA_DisableRXD();
        chainMenu(menuStatisticsView);
        break;
    case EVT_KEY_LONG(KEY_DOWN):
        NMEA_DisableRXD();
        chainMenu(menuMainView);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        if (show_timer == 0) {
            show_timer = 1;
            if (gpstimer <= 0)
                gpstimer = bintime(rbuf[2]);
        }
        else
            show_timer = 0;
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        if ((show_timer == 1) &&(rbuf[2][0]))
            gpstimer = bintime(rbuf[2]);		// get actual GPS time ->resets timer to 00:00
        break;
    }
    /*
    How to use:

    You choose the values to be displayed using the function:

      initval(<number>, <packet>, <value>);
      -------------------------------------

    That means that "<value>" of "<packet>" is stored in the <number> buffer.
    The first <number> is 0.

    Here are the packet names and the associated value names:

    Position packet (beginning with "GGA"): "PACK_GGA"
    value names: "TIM", "LAT", "NOS", "LON", "EOW", "FIX", "SAT", "DIL", "ALT", "MTR", "GEO", "MET", "AGE", "DIF",

    Required minimum packet (beginning with "RMC"): "PACK_RMC"
       value names: "TIM", "NRW", "LT1", "NSO", "LN1", "EWE", "SOG", "COG", "DAT", "MAG", "EAW"

    The buffers are accessed using the macro "VALSTR(<n>)", where "<n>" is "0"
    for the first buffer, and "1" for the second buffer.

    When a value is missing, it is replaced by the contents of val_unknown ("?").
*/

    if (ggareceived)
    {
        gpstime=bintime(rbuf[2]);
        ggareceived=0;
    }

    initval (LONG_BUF(0), PACK_RMC, TIM);					// sets rbuf[0][.]
    initval (LONG_BUF(1), PACK_RMC, DAT);					// sets rbuf[1][.]
    initval (SHORT_BUF(0), PACK_RMC, NRW);				// sets sbuf[0]
    initval (SHORT_BUF(2), PACK_GGA, SAT);				// -> sbuf[2]


    title ('1');
    lcdDrawText        (   2*FW,   1*FH, PSTR("UTC-Time      Sat"));

    if (rbuf[0][0]) {								// show always if data have been received
	  lcdDrawChar   (  19*FW,   1*FH, sbuf[2], 0);				// satellites in view
        lcdDrawSizedText  (   2*FW,   2*FH, &rbuf[0][0], 2, APSIZE);		// hours
        lcdDrawChar   (   6*FW,   2*FH, ':', DBLSIZE);			// ":"
        lcdDrawSizedText  (   8*FW,   2*FH, &rbuf[0][2], 2, APSIZE);		// minutes
        lcdDrawChar   (  12*FW,   2*FH, ':', DBLSIZE);			// ":"
        lcdDrawSizedText  (  14*FW,   2*FH, &rbuf[0][4], 2, APSIZE);		// seconds
    }
    else
        lcdDrawText   (   2*FW,   2*FH, val_unknown, APSIZE);		// "?"

    if ((show_timer == 1) && rbuf[0][0])  {					// show the Timer when data have been received

        lcdDrawText    (   2*FW,   4*FH, PSTR("Timer"));			// display "Timer"
        drawTimer   (   5*FW,   5*FH, (gpstime-gpstimer), DBLSIZE, DBLSIZE);	// display difference as mm:ss
    }
    else
    {
        lcdDrawText      ( 2*FW,   4*FH, PSTR("Date"));			// show the UTC Date	

        if (rbuf[1][0])	{
            lcdDrawSizedText( 2*FW,   5*FH, &rbuf[1][0], 2, APSIZE);		// year
            lcdDrawChar ( 6*FW,   5*FH, '/', DBLSIZE);			// "/" 
            lcdDrawSizedText( 8*FW,   5*FH, &rbuf[1][2], 2, APSIZE);		// month
            lcdDrawChar (12*FW,   5*FH, '/', DBLSIZE);			// "/"
            lcdDrawSizedText(14*FW,   5*FH, &rbuf[1][4], 2, APSIZE);		// day
        }
        else
            lcdDrawText   (   2*FW,   5*FH, val_unknown, APSIZE);		// "?"
    }
}



void menuViewTelemetryNMEA2(event_t event)
{
    static uint8_t ignore_break;


    switch(event)
    {
// Menu navigation
    case EVT_KEY_BREAK(KEY_LEFT):
        if (ignore_break==1)  {
		    ignore_break=0;
                break;}
        chainMenu(menuViewTelemetryNMEA1);
        break;
    case EVT_KEY_BREAK(KEY_RIGHT):
          if (ignore_break==1) {
		     ignore_break=0;
                 break;}
        chainMenu(menuViewTelemetryNMEA3);
        break;
    case EVT_KEY_LONG(KEY_UP):
        NMEA_DisableRXD();
        chainMenu(menuStatisticsView);
        break;
    case EVT_KEY_LONG(KEY_DOWN):
        NMEA_DisableRXD();
        chainMenu(menuMainView);
        break;

//Beep setting
    case EVT_KEY_LONG(KEY_LEFT):
	ignore_break = 1;
        beep_on=0;
        AUDIO_KEY_PRESS();
        break;
    case EVT_KEY_LONG(KEY_RIGHT):
	  ignore_break = 1;
        beep_on=1;
        AUDIO_KEY_PRESS();
        break;

//Altitude setting
	        /*      Set a home position for altitude. Normally used before starting
      	  the model when GPS has got a fix.
	        MENU[short]         		-->	alternating relative and absolute altitudes
		  MENU[long]			-->   set home altitude to current
		  EXIT[long]			-->   reset max altitude to 0

		  Switch ON / OFF short beep with positive lift
		  LEFT[long]			-->	Positive lift Beep off
		  RIGHT[long]			-->	Positive lift Beep on		*/


    case EVT_KEY_BREAK(KEY_MENU):
        if (ignore_break==1)  {
		    ignore_break=0;
                break;}

        if (!home_alt)				// umschalten zwischen absoluter und relativer H�he
            home_alt = save_alt;
        else
            home_alt=0;	

	  if (save_alt==0)			// wenn noch keine Home H�he gesetzt war, wird sie es jetzt, weil sonst
							// das Umschalten keine Wirkung zeigt
	      save_alt = home_alt = abs_alt;			// absolute altitude
        AUDIO_KEY_PRESS();
        break;

    case EVT_KEY_LONG(KEY_MENU):
	ignore_break = 1;
        save_alt = home_alt = abs_alt;	// Home altitude auf aktuelle absolute H�he setzen
        AUDIO_KEY_PRESS();
        break;

    case EVT_KEY_LONG(KEY_EXIT):		// Max Altitude auf 0 zur�cksetzen
	max_alt=0;
        AUDIO_KEY_PRESS();
        break;

    }
    title ('2');
    
    lcdDrawText      (   1*FW,   1*FH, PSTR("Altitude Sat   Max"));


    lcdDrawText      (   16*FW,   3*FH, PSTR("Home"));
    lcdDrawText      (   2*FW,   4*FH, PSTR("Lift") );

    lcdDrawText      (   16*FW,   5*FH, PSTR("Beep") );
    if (beep_on==1)
        lcdDrawText      (   18*FW,   6*FH, PSTR("ON") );
    
    else	
        lcdDrawText      (   17*FW,   6*FH, PSTR("OFF") );


    lcdDrawNumber(  20*FW,   4*FH, home_alt, PREC1, 6);		// display home_alt, small characters 

    if (xpack[0] != PACK_GGA)
        ggareceived = 0;

    initval (LONG_BUF(0), PACK_GGA, ALT);				// -> rbuf[0]
    initval (LONG_BUF(1), PACK_GGA, GEO);				// -> rbuf[1]
    initval (SHORT_BUF(0), PACK_GGA, MTR);			// -> sbuf[0]
    initval (SHORT_BUF(1), PACK_GGA, FIX);			// -> sbuf[1]
    initval (SHORT_BUF(2), PACK_GGA, SAT);			// -> sbuf[2]

    if (ggareceived)   // at least one second has elapsed
    {
        ggareceived = 0;

        /*      ALT and GEO have one single digit following the decimal point
        e.g. ALT=359.7   GEO=47.7
        The altitude over mean sea level is to be calculated as:
        altitude minus geoidal separation  
        */

        abs_alt = binary(rbuf[0]) - binary(rbuf[1]);		// alt - geo  that is absolute altitude

	  if (abs_alt> max_alt) max_alt=abs_alt;			// hold max altitude relative to 0 m

	  rel_alt=abs_alt - home_alt;					// alt - geo - home  altitude relative to home


        lift_alt = rel_alt - prev_alt;
        prev_alt = rel_alt;

        if ((lift_alt >= 0) && (sbuf[1]>0x30) && beep_on)			// GGA record must have Fix> 0	
            AUDIO_KEY_PRESS(); 						// short blip for non negative lift

    }

    if (rbuf[0][0])	 {						
	  lcdDrawChar   (  13*FW,   1*FH, sbuf[2], 0);				// satellites in view

	  if (sbuf[1]>0x30)	 {							// & GGA has FIX > 0


	        lcdDrawNumber(  10*FW,   2*FH, rel_alt, DBLSIZE|PREC1, 7);	// altitude
	
		  if (home_alt >= 0) 
			  lcdDrawNumber(  20*FW,   2*FH, (max_alt-home_alt), PREC1, 6);	// display small characters
		  else
			  lcdDrawNumber(  20*FW,   2*FH, max_alt, PREC1, 6);			// display small characters
	

	        lcdDrawChar   (  11*FW,   3*FH, sbuf[0], 0);				// dimension [m]

      	  lcdDrawNumber(  10*FW,   5*FH, lift_alt, DBLSIZE|PREC1, 6);	// lift
	        lcdDrawChar   (  11*FW,   6*FH, sbuf[0], 0);				// dimension [m/S]
      	  lcdDrawText    (  12*FW,   6*FH, PSTR("/S") );
		}
    }
    else {
        lcdDrawText    (   2*FW,   2*FH, val_unknown, APSIZE);
        lcdDrawText    (   2*FW,   5*FH, val_unknown, APSIZE);
    }
}



void menuViewTelemetryNMEA3(event_t event)
{
    switch(event)
    {
    case EVT_KEY_BREAK(KEY_LEFT):
        chainMenu(menuViewTelemetryNMEA2);
        break;
    case EVT_KEY_BREAK(KEY_RIGHT):
        chainMenu(menuViewTelemetryNMEA4);
        break;
    case EVT_KEY_LONG(KEY_UP):
        NMEA_DisableRXD();
        chainMenu(menuStatisticsView);
        break;
    case EVT_KEY_LONG(KEY_DOWN):
        NMEA_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    initval (LONG_BUF(0), PACK_RMC, SOG);
    initval (LONG_BUF(1), PACK_RMC, COG);
    initval (SHORT_BUF(2), PACK_GGA, SAT);			// -> sbuf[2]
    title ('3');
    lcdDrawText        (   0*FW,   1*FH, PSTR("GrndSpeed[knt]  Sat"));
    if (rbuf[0][0])				// if first position is 00, buffer is empty, taken as false 
    {							// any other value is true
        uint8_t i = 0;
        while (rbuf[0][i])
        {
            if (rbuf[0][i] == '.')		// find decimal point and insert End of String 3 positions higher
            {
                rbuf[0][i+3] = 0;
                break;
            }
            i++;
        }
        lcdDrawText   (   2*FW,   2*FH, VALSTR(0), APSIZE);		// speed over ground
    }
    else
        lcdDrawText   (   2*FW,   2*FH, val_unknown, APSIZE);

    lcdDrawChar   (  19*FW,   1*FH, sbuf[2], 0);			// satellites in view

    lcdDrawText        (   1*FW,   4*FH, PSTR("Course over ground") );
    lcdDrawText       (   2*FW,   5*FH, VALSTR(1), APSIZE);		// course over ground
}



void menuViewTelemetryNMEA4(event_t event)
{
    switch(event)						// new event received, branch accordingly
    {
    case EVT_KEY_BREAK(KEY_LEFT):
        chainMenu(menuViewTelemetryNMEA3);
        break;
    case EVT_KEY_BREAK(KEY_RIGHT):
        chainMenu(menuViewTelemetryNMEA1);
        break;
    case EVT_KEY_LONG(KEY_UP):
        NMEA_DisableRXD();
        chainMenu(menuStatisticsView);
        break;
    case EVT_KEY_LONG(KEY_DOWN):
        NMEA_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    // expecting LAT value in POS packet to be stored in the first buffer
    initval (LONG_BUF(0), PACK_GGA, LAT);
    initval (SHORT_BUF(0), PACK_GGA, NOS);
    // and LON value in POS packet stored in the second buffer
    initval (LONG_BUF(1), PACK_GGA, LON);
    initval (SHORT_BUF(1), PACK_GGA, EOW);
    initval (SHORT_BUF(2), PACK_GGA, SAT);			// -> sbuf[2]
    // title of the screen
    title ('4');
    lcdDrawText        (   3*FW,   1*FH, PSTR("Latitude     Sat"));    // line 1 column 3
    // first buffer into line 2 column 2
    if (rbuf[0][0])
    {
        lcdDrawChar   (  13*FW,   1*FH, sbuf[0], 0);          // N or S
	  lcdDrawChar   (  19*FW,   1*FH, sbuf[2], 0);				// satellites in view
        lcdDrawSizedText  (   1*FW,   2*FH, rbuf[0], 2, APSIZE);
        lcdDrawChar   (   5*FW,   2*FH, '@',0);
        lcdDrawText   (   6*FW,   2*FH, &rbuf[0][2], APSIZE);	// minutes with small decimal point
    }
    else
        lcdDrawText   (   2*FW,   2*FH, val_unknown, APSIZE);
    lcdDrawText        (   3*FW,   4*FH, PSTR("Longitude"));   // line 4 column 5
    // second buffer into line 5 column 2
    if (rbuf[0][0])
    {
        lcdDrawChar   (  13*FW,   4*FH, sbuf[1], 0);          // E or W
        lcdDrawSizedText  (   0*FW,   5*FH, rbuf[1], 3, APSIZE);
        lcdDrawChar   (   6*FW,   5*FH, '@',0);
        lcdDrawText   (   7*FW,   5*FH, &rbuf[1][3], APSIZE);	// minutes with small decimal point

    }
    else
        lcdDrawText   (   2*FW,   5*FH, val_unknown, APSIZE);
}

void title(char x)
{
    lcdDrawText (0*FW, 0*FH, PSTR("  GPS NMEA data ?/4  "), INVERS);
    lcdDrawChar(16*FW, 0*FH, x, INVERS);
}

void initval(uint8_t num, uint8_t pack, uint8_t val)
{
    if (xpack[num] != pack || xval[num] != val)
    {
        if (num < NB_LONG_BUF) {
            ibuf[num] = rbuf[num][0] = 0;
        }
        else
            sbuf[num-NB_LONG_BUF] = '?';
        xpack[num] = pack;
        xval[num] = val;
        state = WAIT_PACKET;			// synchronize to the next packet
    }
}

int32_t binary (char *str)
{
    int32_t numval = 0;
    uint8_t sign = 0;

    while (*str) {
        if (*str == '-')
            sign = 1;
        else if (*str >= '0' && *str <= '9')
            numval = numval * 10 + (*str - '0');
        str++;
    }
    if (sign)
        numval = -numval;
    return numval;
}

int32_t bintime (char *str)
{
    int32_t numval=0;

    if (*str) {
        numval = ((str[0] - '0') * 10l) + (str[1] - '0');					// hours
        numval = numval * 3600l;
        numval = numval + (((  (str[2] - '0') * 10l) + (str[3] - '0')) * 60l);	// minutes
        numval = numval + ((str[4] - '0') * 10l) + (str[5] - '0');			// seconds
    }
    return numval;
}
/*
Without NMEA:

Size after:
AVR Memory Usage
----------------
Device: atmega64

Program:   54226 bytes (82.7% Full)
(.text + .data + .bootloader)

Data:       3440 bytes (84.0% Full)
(.data + .bss + .noinit)

----------------------------------

With NMEA:

Size after:
AVR Memory Usage
----------------
Device: atmega64

Program:   57098 bytes (87.1% Full)
(.text + .data + .bootloader)

Data:       3524 bytes (86.0% Full)
(.data + .bss + .noinit)
*/
