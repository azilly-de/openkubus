/* memxor.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
 * File:        memxor.S
 * Author:      Daniel Otte
 * Date:        2008-08-07
 * License:     GPLv3 or later
 * Description: memxor, XORing one block into another
 * 
 */
 
/*
 * void memxor(void* dest, const void* src, uint16_t n);
 */
 /*
  * param dest is passed in r24:r25
  * param src  is passed in r22:r23
  * param n    is passed in r20:r21
  */
.global memxor
memxor:
	movw r30, r24
	movw r26, r22
	movw r24, r20
	tst r24
	brne 1f
	tst r25
	breq 2f
1:
	ld r20, X+
	ld r21, Z
	eor r20, r21
	st Z+, r20
	sbiw r24, 1
	brne 1b
2:
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	

