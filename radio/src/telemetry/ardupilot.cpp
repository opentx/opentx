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

#define NB_BUF 2
#define LG_BUF 11
#define VALSTR(val)  (rbuf[val][0] ? rbuf[val] : val_unknown)
#define APSIZE (BSS | DBLSIZE)

/*    Received data

Data are received as packets, each packet is identified by a prefix of three
characters and is ended by three stars. Some packets can contain more than
one value, the values are ended by a comma.


Position packet:
!!!LAT:0123456789,LON:9876543210,SPD:123,CRT:1,ALT:123456,ALH:654321,CRS:123,BER:123,WPN:12,DST:12345,BTV:12.4567 9012,RSP:123,TOW:123456,***
...----1--------------2--------------3-------4-----5----------6----------7-------8-------9------10--------11-----------12----------13-----END
       1st value      2nd value

Attitude packet:
+++ASP:123,THH:123,RLL:123,PCH:123,***
...----1-------2-------3-------4---END

Mode change packet:
###STABILIZE***
...1--------END

Waypoint packet:
%%%12 45***
...1----END

Alert packet:
XXXAlert Text alert***
...1---------------END

Performance packet:
PPPThis is performant***
...1--------------END
*/

// Position packet prefix
#define PACK_POS 0x21
// value occurence number in this packet
#define LAT 1
#define LON 2
#define SPD 3
#define CRT 4
#define ALT 5
#define ALH 6
#define CRS 7
#define BER 8
#define WPN 9
#define DST 10
#define BTV 11
#define RSP 12
#define TOW 13

#define PACK_ATT 0x2b
// value occurence number in this packet
#define ASP 1
#define THH 2
#define RLL 3
#define PCH 4

#define PACK_MOD 0x23
// value occurence number in this packet
#define MOD 1

#define PACK_WPC 0x25
// value occurence number in this packet
#define WPC 1

#define PACK_ALR 0x58
// value occurence number in this packet
#define ALR 1

#define PACK_PRF 0x50
// value occurence number in this packet
#define PRF 1

// end of packet
#define PACK_END 0x2a
// end of value
#define VAL_END 0x2c
// end of title
#define TITLE_END 0x3a

// stateful machine

//   Since the packets are sent continuously, we need to synchronize on the
//   reception of the three chars prefixing a packet, whatever they are.

// states values
#define WAIT_PACKET     1
#define WAIT_PACK_POS1  2
#define WAIT_PACK_POS2  3
#define WAIT_PACK_POS3  4
#define WAIT_PACK_ATT1  5
#define WAIT_PACK_ATT2  6
#define WAIT_PACK_ATT3  7
#define WAIT_PACK_MOD1  8
#define WAIT_PACK_MOD2  9
#define WAIT_PACK_MOD3 10
#define WAIT_PACK_WPC1 11
#define WAIT_PACK_WPC2 12
#define WAIT_PACK_WPC3 13
#define WAIT_PACK_ALR1 14
#define WAIT_PACK_ALR2 15
#define WAIT_PACK_ALR3 16
#define WAIT_PACK_PRF1 17
#define WAIT_PACK_PRF2 18
#define WAIT_PACK_PRF3 19
#define FLUSH_TITLE1   20
#define FLUSH_TITLE2   21
#define FLUSH_TITLE3   22
#define FLUSH_TITLE4   23
#define READ_VALUE     24

uint8_t i;							// working variable
uint8_t state;						// currrent state
uint8_t rval, rpack;				// received items
uint8_t xval[NB_BUF];               // expected value
uint8_t xpack[NB_BUF];			    // expected packet
uint8_t ibuf[NB_BUF];				// subscripts on buffers values
char rbuf[NB_BUF][LG_BUF];		    // receive buffers
const char val_unknown[] = "?";

void menuViewTelemetryArduPilot1(event_t event);
void menuViewTelemetryArduPilot2(event_t event);
void menuViewTelemetryArduPilot3(event_t event);
void menuViewTelemetryArduPilot4(event_t event);
void menuViewTelemetryArduPilot5(event_t event);
void menuViewTelemetryArduPilot6(event_t event);
void menuViewTelemetryArduPilot7(event_t event);
void menuViewTelemetryArduPilot8(event_t event);
void title(char x);
void initval(uint8_t num, uint8_t pack, uint8_t val);

#ifndef SIMU
ISR (USART0_RX_vect)
{
    uint8_t rl;
//    uint8_t rh;                         //USART control and Status Register 0 B
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
        state = WAIT_PACKET;			// restart on error
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
        case PACK_POS:					// found a new POS packet
            state = WAIT_PACK_POS2;		// wait for the 2nd char
            break;
        case PACK_ATT:					// found a new ATT packet
            state = WAIT_PACK_ATT2;		// wait for the 2nd char
            break;
        case PACK_MOD:					// found a new MOD packet
            state = WAIT_PACK_MOD2;		// wait for the 2nd char
            break;
        case PACK_WPC:					// found a new WPC packet
            state = WAIT_PACK_WPC2;		// wait for the 2nd char
            break;
        case PACK_ALR:					// found a new ALR packet
            state = WAIT_PACK_ALR2;		// wait for the 2nd char
            break;
        case PACK_PRF:					// found a new PRF packet
            state = WAIT_PACK_PRF2;		// wait for the 2nd char
            break;
        }
        break;
    case WAIT_PACK_POS2:				// wait for 2nd char
        if (rl == PACK_POS)
            state = WAIT_PACK_POS3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_POS3:				// wait for 3rd char
        if (rl == PACK_POS)				// found
        {
            state = FLUSH_TITLE1;		// flush title "LAT:"
            rpack = PACK_POS;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_ATT2:				// wait for 2nd char
        if (rl == PACK_ATT)
            state = WAIT_PACK_ATT3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_ATT3:				// wait for 3rd char
        if (rl == PACK_ATT)
        {
            state = FLUSH_TITLE1;		// flush title "ASP:"
            rpack = PACK_ATT;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_MOD2:				// wait for 2nd char
        if (rl == PACK_MOD)
            state = WAIT_PACK_MOD3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_MOD3:				// wait for 3rd char
        if (rl == PACK_MOD)
        {
            state = READ_VALUE;		    // ready to read values from PACK_MOD packet
            rpack = PACK_MOD;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_WPC2:				// wait for 2nd char
        if (rl == PACK_WPC)
            state = WAIT_PACK_WPC3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_WPC3:				// wait for 3rd char
        if (rl == PACK_WPC)
        {
            state = READ_VALUE;		    // ready to read values from PACK_WPC packet
            rpack = PACK_WPC;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_ALR2:				// wait for 2nd char
        if (rl == PACK_ALR)
            state = WAIT_PACK_ALR3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_ALR3:				// wait for 3rd char
        if (rl == PACK_ALR)
        {
            state = READ_VALUE;		    // ready to read values from PACK_ALR packet
            rpack = PACK_ALR;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_PRF2:				// wait for 2nd char
        if (rl == PACK_PRF)
            state = WAIT_PACK_PRF3;
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case WAIT_PACK_PRF3:				// wait for 3rd char
        if (rl == PACK_PRF)
        {
            state = READ_VALUE;		    // ready to read values from PACK_PRF packet
            rpack = PACK_PRF;
            rval = 1;
            for (i = 0; i < NB_BUF; i++)
                ibuf[i] = 0;
        }
        else
            state = WAIT_PACKET;		// restart if not found
        break;
    case FLUSH_TITLE1:					// wait for 1st char of title
        state = FLUSH_TITLE2;
        break;
    case FLUSH_TITLE2:					// wait for 2nd char
        state = FLUSH_TITLE3;
        break;
    case FLUSH_TITLE3:					// wait for 3rd char
        state = FLUSH_TITLE4;
        break;
    case FLUSH_TITLE4:					// wait for 4th char
        if (rl == TITLE_END)
            state = READ_VALUE;
        else
            state = WAIT_PACKET;		// was not a title, wait for the next packet
        break;
    case READ_VALUE:
        switch (rl)
        {
        case PACK_END:
            state = WAIT_PACKET;		// packet completed, wait for the next packet
            break;
        case VAL_END:					// comma found, value completed
            state = FLUSH_TITLE1;       // flush next title
            rval++;						// and get next value
            break;
        default:						// store the char in the corresponding buffer
            for (i = 0; i < NB_BUF; i++)
            {							// is it the expected value in the expected packet ?
                if (rpack == xpack[i] && rval == xval[i] && ibuf[i] < LG_BUF - 1)
                {						// yes, store the char
                    rbuf[i] [ibuf[i]] = rl;
                    ibuf[i]++;
                    rbuf[i] [ibuf[i]] = 0;
                }
            }
            break;
        }
        break;
    }
}
#endif

void ARDUPILOT_Init (void)
{
    DDRE  &= ~(1 << DDE0);              // set RXD0 pin as input
    PORTE &= ~(1 << PORTE0);            // disable pullup on RXD0 pin

#ifndef SIMU
                            // switch (Telem_baud)
                            // {
                            // case 1:
#undef BAUD
#define BAUD 38400
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
        UDR0;						    // flush receive buffer
#endif
}

void ARDUPILOT_DisableTXD (void)
{
    UCSR0B &= ~(1 << TXEN0);            // disable TX
}

void ARDUPILOT_EnableTXD (void)
{
    UCSR0B |=  (1 << TXEN0);            // enable TX
}

void ARDUPILOT_DisableRXD (void)
{
    UCSR0B &= ~(1 << RXEN0);            // disable RX
    UCSR0B &= ~(1 << RXCIE0);           // disable Interrupt
}

void ARDUPILOT_EnableRXD (void)
{
    for (i = 0; i < NB_BUF; i++)
    {
        ibuf[i] = 0;
        rbuf[i][0] = 0;
        xpack[i] = 0;
        xval[i] = 0;
    }
    state = WAIT_PACKET;			    // wait for the next packet
    UCSR0B |=  (1 << RXEN0);		    // enable RX
    UCSR0B |=  (1 << RXCIE0);		    // enable Interrupt
}

void menuViewTelemetryArduPilot(event_t event)
{
    menuViewTelemetryArduPilot1(event);
}

// Start of ArduPilot menus 1-8 <<<<<<<<<<<<<<<<<<<<<<<<<<<

void menuViewTelemetryArduPilot1(event_t event)
{
    switch(event)						// new event received, branch accordingly
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot8);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot2);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        ARDUPILOT_DisableRXD();
        chainMenu(menuStatisticsView);
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
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

    Position packet (beginning with "!!!"): "PACK_POS"
       value names: "LAT", "LON", "SPD", "CRT", "ALT", "ALH", "CRS", "BER",
       "WPN", "DST", "BTV", "RSP", "TOW"

    Attitude packet (beginning with "+++"): "PACK_ATT"
       value names: "ASP", "THH", "RLL", "PCH"

    Mode change packet (beginning with "###"): "PACK_MOD"
       value name: "MOD"

    Waypoint packet (beginning with "%%%"): "PACK_WPC"
       value name: "WPC"

    Alert packet (beginning with "XXX"): "PACK_ALR"
       value name: "ALR"

    Performance packet (beginning with "PPP"): "PACK_PRF"
       value name: "PRF"

    The buffers are accessed using the macro "VALSTR(<n>)", where "<n>" is "0"
    for the first buffer, and "1" for the second buffer.

    When a value is missing, it is replaced by the contents of val_unknown ("?").
*/
// expecting LAT value in POS packet to be stored in the first buffer
    initval (0, PACK_POS, LAT);
// and LON value in POS packet stored in the second buffer
    initval (1, PACK_POS, LON);
// title of the screen
    title ('1');
    lcdDrawText(5*FW, 1*FH, PSTR(" Latitude"));   // line 1 column 5
// first buffer into line 2 column 2
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(5*FW, 4*FH, PSTR(" Longitude"));  // line 4 column 5
// second buffer into line 5 column 2
    lcdDrawText (1*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot2(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot1);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot3);
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    initval (0, PACK_POS, SPD);
    initval (1, PACK_POS, CRT);
    title ('2');  
    lcdDrawText(1*FW, 1*FH, PSTR(" Ground speed"));    
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Climb rate") );
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot3(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot2);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot4);
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    initval (0, PACK_POS, ALT);
    initval (1, PACK_POS, ALH);
    title ('3');  
    lcdDrawText(1*FW, 1*FH, PSTR(" Altitude"));    
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Altitude Hold") );
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot4(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot3);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot5);
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
    break;
    }
    initval (0, PACK_POS, CRS);
    initval (1, PACK_POS, BER);
    title ('4'); 
    lcdDrawText(1*FW, 1*FH, PSTR(" Course"));    
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Bearing"));
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot5(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot4);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot6);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
    break;
    }
    initval (0, PACK_POS, WPN);
    initval (1, PACK_POS, DST);
    title ('5'); 
    lcdDrawText(1*FW, 1*FH, PSTR(" Way Point # "));    
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Distance "));
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot6(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot5);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot7);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    initval (0, PACK_ATT, ASP);
    initval (1, PACK_ATT, THH);
    title ('6'); 
    lcdDrawText(1*FW, 1*FH, PSTR(" Air Speed "));    
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Climb Rate "));
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot7(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot6);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot8);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
    break;
    }
    initval (0, PACK_ATT, RLL);
    initval (1, PACK_ATT, PCH);
    title ('7'); 
    lcdDrawText(1*FW, 1*FH, PSTR(" Roll Angle"));   
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" Pitch Angle"));
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void menuViewTelemetryArduPilot8(event_t event)
{
    switch(event)
    {
    case EVT_KEY_FIRST(KEY_UP):
        chainMenu(menuViewTelemetryArduPilot7);
        break;
    case EVT_KEY_FIRST(KEY_DOWN):
        chainMenu(menuViewTelemetryArduPilot1);
        break;
    case EVT_KEY_FIRST(KEY_MENU):
        break;
    case EVT_KEY_FIRST(KEY_EXIT):
        ARDUPILOT_DisableRXD();
        chainMenu(menuMainView);
        break;
    }
    initval (0, PACK_MOD, MOD);
    initval (1, PACK_WPC, WPC);
    title ('8');
    lcdDrawText(1*FW, 1*FH, PSTR(" ArduPilot Mode"));
    lcdDrawText (2*FW, 2*FH, VALSTR(0), APSIZE);
    lcdDrawText(1*FW, 4*FH, PSTR(" RTL Distance"));
    lcdDrawText (2*FW, 5*FH, VALSTR(1), APSIZE);
}

void title(char x)
{
    lcdDrawText (0, 0, PSTR(" ARDU PILOT Mega ?/8 "), INVERS);
    lcdDrawChar(17*FW, 0*FH, x, INVERS);
}

void initval(uint8_t num, uint8_t pack, uint8_t val)
{
    if (xpack[num] != pack || xval[num] != val)
    {
        ibuf[num] = rbuf[num][0] = 0;
        xpack[num] = pack;
        xval[num] = val;
        state = WAIT_PACKET;			// synchronize to the next packet
    }
}

/*
Without ArduPilot:
Size after:
er9x.elf  :
section             size      addr
.data                164   8388864
.text              50634         0
.bss                3485   8389028
----------------------------------
With ArduPilot:
Size after:
er9x.elf  :
section             size      addr
.data                166   8388864
.text              53026        0
.bss                3517   8389030
*/
