/*
 *  Copyright Droids Corporation, Microb Technology, Eirbot (2005)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Revision : $Id: base64.c,v 1.2.8.1 2008/10/07 17:43:03 zer0 Exp $
 *
 */

#include <string.h>
#include <stdint.h>

char my_base64(uint8_t pos)
{
  // ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/
  if(pos >= 0 && pos <= 25)
    return pos + 'A';
  else if(pos >= 26 && pos <= 51)
    return pos + 'a' - 26;
  else if(pos >= 52 && pos <= 61)
    return pos + '0' - 52;
  else if(pos == 62)
    return '+';
  else if(pos == 63)
    return '/';

    return '\0';
}

/* return -1 if out buffer is too small */
/* else return number of written bytes in out buffer */
int raw_to_base64(char *in, int len_in, char * out, int len_out)
{
  int n_in;
  int counter=0;
  int n_out=0;
  unsigned int buf=0;

  for (n_in=0 ; n_in<len_in && n_out<len_out ; n_in++)  {
    buf <<= 8;
    buf += (unsigned char)(in[n_in]);
    counter+=8;
    
    while(counter>=6) {
      if (n_out>=len_out)
        return -1;
      out[n_out++] = my_base64((buf>>(counter-6)) & 0x3F);
      counter-=6;
    }
  }

  /* should we add padding ? */
  if (counter) {
    if (n_out >= len_out)
      return -1;

    out[n_out++] =  my_base64((buf<<(6-counter)) & 0x3F);
    
    if (n_out >= len_out)
      return -1;

    /* one '=' if counter==4 (counter can be 4 or 2) */
    out[n_out++] = '=';

    if (n_out >= len_out)
      return -1;

    /* two '=' if cpt==2 */
    if (counter==2) {
      out[n_out++] = '=';
    }
  }
  
  return n_out;
}
