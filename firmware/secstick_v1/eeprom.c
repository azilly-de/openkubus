#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>

#define EEPROM_READY() while(EECR & (1 << EEWE));

uint8_t eeprom_write(uint16_t addr, uint8_t * buf, uint16_t length)
{
  uint8_t i;

  for(i = 0; i < length; i++)
  {
    // wait for eeprom
    EEPROM_READY();

    EEAR = addr++;
    EEDR = buf[i];

    EECR |= (1 << EEMWE);
    EECR |= (1 << EEWE);
  }

  return 1;
}

uint8_t eeprom_read(uint16_t addr, uint8_t * buf, uint16_t length)
{
  uint8_t i;


  for(i = 0; i < length; i++)
  {
    // wait for eeprom
    EEPROM_READY();

    // set addr
    EEAR = addr++;

    EECR |= (1 << EERE);

    buf[i] = EEDR;
  }

  return 1;
}
