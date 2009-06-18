/*
 * (c) 2009, Michael Hartmann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
