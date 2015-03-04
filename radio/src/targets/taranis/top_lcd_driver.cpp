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

uint8_t top_lcd_data[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void delay1_7us()
{
  delay_01us(17);
}

void ht1621SendCommand(uint8_t chip, unsigned char command)
{
  if (chip)
    CS2_LOW;
  else
    CS1_LOW;

  delay1_7us();
  WR_LOW;      //PRESENT 100 COMMAND CODE
  delay1_7us();
  DATA_HIGH;
  delay1_7us();
  WR_HIGH;
  delay1_7us();
  WR_LOW;
  delay1_7us();
  DATA_LOW;
  delay1_7us();
  WR_HIGH;
  delay1_7us();

  WR_LOW;
  delay1_7us();
  DATA_LOW;
  delay1_7us();
  WR_HIGH;
  delay1_7us();

  for (int i=0; i<8; i++) {
    WR_LOW;
    delay1_7us();
    if ((command & 0x80) !=0)
    {
      DATA_HIGH;
      delay1_7us();
    }
    else
    {
      DATA_LOW;
      delay1_7us();
    }
    WR_HIGH;
    delay1_7us();
    command = command << 1;
  }
  WR_LOW;
  delay1_7us();
  WR_HIGH;
  delay1_7us();

  if (chip)
    CS2_HIGH;
  else
    CS1_HIGH;

  delay1_7us();
}

void ht1621WrData(uint8_t data, uint8_t count)
{
  while (count) {
    WR_LOW;
    if (data & 0x80)
      DATA_HIGH;
    else
      DATA_LOW;
    delay1_7us();
    WR_HIGH;
    delay1_7us();
    data <<= 1;
    count -= 1;
  }
}

void ht1621WrAllData(uint8_t chip, uint8_t *pData)
{
  if (chip)
    CS2_LOW;
  else
    CS1_LOW;
  delay1_7us();
  ht1621WrData(0xa0, 3);
  ht1621WrData(0, 6); // HT1621 6 bit,left 2 bit;
  for (int i=0; i<16; i++) {
    ht1621WrData(*pData++, 8);
  }
  if (chip)
    CS2_HIGH;
  else
    CS1_HIGH;
  delay1_7us();
}

void topLcdInit()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_VA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PIN_VA_DATA | PIN_VA_WR |PIN_VA_BL|PIN_VA_CS|PIN_VA_CS2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_VA, &GPIO_InitStructure);

  VA_BL_ON;

  ht1621SendCommand(0, 0x03);
  ht1621SendCommand(0, 0x01);
  ht1621SendCommand(0, 0x29);

  ht1621SendCommand(1, 0x03);
  ht1621SendCommand(1, 0x01);
  ht1621SendCommand(1, 0x29);
}

void topLcdRefresh()
{
  ht1621WrAllData(0, top_lcd_data);
  ht1621WrAllData(1, top_lcd_data);
}
