/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
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

#include "../../opentx.h"

volatile uint32_t Spi_complete;
uint8_t Spi_tx_buf[24] ;

uint32_t eepromTransmitData(register uint8_t *command, register uint8_t *tx, register uint8_t *rx, register uint32_t comlen, register uint32_t count )
{
#ifndef SIMU

  register Spi *spiptr ;
  register uint32_t condition ;
  static uint8_t discard_rx_command[4] ;

//  PMC->PMC_PCER0 |= 0x00200000L ;             // Enable peripheral clock to SPI

  Spi_complete = 0 ;
  if ( comlen > 4 )
  {
    Spi_complete = 1 ;
    return 0x4FFFF ;
  }
  condition = SPI_SR_TXEMPTY ;
  spiptr = SPI ;
  spiptr->SPI_CR = 1 ;                    // Enable
  (void) spiptr->SPI_RDR ;                // Dump any rx data
  (void) spiptr->SPI_SR ;                 // Clear error flags
  spiptr->SPI_RPR = (uint32_t)discard_rx_command ;
  spiptr->SPI_RCR = comlen ;
  if ( rx )
  {
    spiptr->SPI_RNPR = (uint32_t)rx ;
    spiptr->SPI_RNCR = count ;
    condition = SPI_SR_RXBUFF ;
  }
  spiptr->SPI_TPR = (uint32_t)command ;
  spiptr->SPI_TCR = comlen ;
  if ( tx )
  {
    spiptr->SPI_TNPR = (uint32_t)tx ;
  }
  else
  {
    spiptr->SPI_TNPR = (uint32_t)rx ;
  }
  spiptr->SPI_TNCR = count ;

  spiptr->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN ;    // Start transfers

  // Wait for things to get started, avoids early interrupt
  for ( count = 0 ; count < 1000 ; count += 1 )
  {
    if ( ( spiptr->SPI_SR & SPI_SR_TXEMPTY ) == 0 )
    {
      break ;
    }
  }
  spiptr->SPI_IER = condition ;

#endif

  return 0 ;
}

uint8_t eepromTransmitByte(uint8_t out, bool skipFirst)
{
  register Spi *spiptr;
  register uint32_t delay;

#if defined(SIMU)
  return 0;
#endif

  spiptr = SPI;
  spiptr->SPI_CR = 1; // Enable
  (void) spiptr->SPI_RDR; // Dump any rx data

  spiptr->SPI_TDR = out;

  delay = 0;
  while ((spiptr->SPI_SR & SPI_SR_RDRF) == 0) {
    // wait for received
    if (++delay > 10000) {
      break;
    }
  }
  
  if (skipFirst) {
    (void) spiptr->SPI_RDR; // Dump the rx data
    spiptr->SPI_TDR = 0;
    delay = 0;
    while ((spiptr->SPI_SR & SPI_SR_RDRF) == 0) {
      // wait for received
      if (++delay > 10000) {
        break;
      }
    }
  }
  
  spiptr->SPI_CR = 2; // Disable
  return spiptr->SPI_RDR ;
}

enum EepromCommand {
  COMMAND_WRITE_STATUS_REGISTER = 0x01,
  COMMAND_BYTE_PROGRAM = 0x02,
  COMMAND_READ_ARRAY = 0x03,
  COMMAND_READ_STATUS = 0x05,
  COMMAND_WRITE_ENABLE = 0x06,
  COMMAND_BLOCK_ERASE = 0x20,
};

void eepromPrepareCommand(EepromCommand command, uint32_t address)
{
  uint8_t * p = Spi_tx_buf;
  *p = command;
  *(p+1) = address >> 16;
  *(p+2) = address >> 8;
  *(p+3) = address;
}

uint32_t eepromReadStatus()
{
  return eepromTransmitByte(COMMAND_READ_STATUS, true);
}

void eepromWriteStatusRegister()
{
  eepromTransmitByte(COMMAND_WRITE_STATUS_REGISTER, true);
}

void eepromByteProgram(uint32_t address, uint8_t * buffer, uint32_t size)
{
  eepromPrepareCommand(COMMAND_BYTE_PROGRAM, address);
  eepromTransmitData(Spi_tx_buf, buffer, 0, 4, size);
}

void eepromReadArray(uint32_t address, uint8_t * buffer, uint32_t size)
{
  eepromPrepareCommand(COMMAND_READ_ARRAY, address);
  eepromTransmitData(Spi_tx_buf, 0, buffer, 4, size);
}

void eepromWriteEnable()
{
  eepromTransmitByte(COMMAND_WRITE_ENABLE, false);
}

void eepromBlockErase(uint32_t address)
{
  eepromPrepareCommand(COMMAND_BLOCK_ERASE, address);
  eepromTransmitData(Spi_tx_buf, 0, 0, 4, 0);
}

// SPI i/f to EEPROM (4Mb)
// Peripheral ID 21 (0x00200000)
// Connections:
// SS   PA11 (peripheral A)
// MISO PA12 (peripheral A)
// MOSI PA13 (peripheral A)
// SCK  PA14 (peripheral A)
// Set clock to 3 MHz, AT25 device is rated to 70MHz, 18MHz would be better
void eepromInit()
{
  register Spi *spiptr ;
  register uint32_t timer ;

  PMC->PMC_PCER0 |= 0x00200000L ;               // Enable peripheral clock to SPI
  /* Configure PIO */
  configure_pins( 0x00007800, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_A | PIN_PORTA | PIN_NO_PULLUP ) ;

  spiptr = SPI ;
  timer = ( Master_frequency / 3000000 ) << 8 ;           // Baud rate 3Mb/s
  spiptr->SPI_MR = 0x14000011 ;                           // 0001 0100 0000 0000 0000 0000 0001 0001 Master
  spiptr->SPI_CSR[0] = 0x01180009 | timer ;               // 0000 0001 0001 1000 xxxx xxxx 0000 1001
  NVIC_EnableIRQ(SPI_IRQn) ;

  eepromWriteEnable();
  eepromWriteStatusRegister();
}

#ifndef SIMU
extern "C" void SPI_IRQHandler()
{
  register Spi *spiptr ;

  spiptr = SPI ;
  SPI->SPI_IDR = 0x07FF ;                 // All interrupts off
  spiptr->SPI_CR = 2 ;                    // Disable
  (void) spiptr->SPI_RDR ;                // Dump any rx data
  (void) spiptr->SPI_SR ;                 // Clear error flags
  spiptr->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS ;  // Stop tramsfers
  Spi_complete = 1 ;                                      // Indicate completion

// Power save
//  PMC->PMC_PCER0 &= ~0x00200000L ;      // Disable peripheral clock to SPI
}
#endif
