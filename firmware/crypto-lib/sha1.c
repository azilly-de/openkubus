/* sha1.c */
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
/**
 * \file	sha1.c
 * \author	Daniel Otte
 * \date	2006-10-08
 * \license GPLv3 or later
 * \brief SHA-1 implementation.
 * 
 */
 
#include <string.h> /* memcpy & co */
#include <stdint.h>
#include "config.h"
#undef DEBUG
#include "debug.h"
#include "sha1.h"

#define LITTLE_ENDIAN

/********************************************************************************************************/
 
/**
 * \brief initialises given SHA-1 context
 * 
 */
void sha1_init(sha1_ctx_t *state){
	DEBUG_S("\r\nSHA1_INIT");
	state->h[0] = 0x67452301;
	state->h[1] = 0xefcdab89;
	state->h[2] = 0x98badcfe;
	state->h[3] = 0x10325476;
	state->h[4] = 0xc3d2e1f0;
	state->length = 0;
}

/********************************************************************************************************/
/* some helping functions */
uint32_t rotl32(uint32_t n, uint8_t bits){
	return ((n<<bits) | (n>>(32-bits)));
}

uint32_t change_endian32(uint32_t x){
	return (((x)<<24) | ((x)>>24) | (((x)& 0x0000ff00)<<8) | (((x)& 0x00ff0000)>>8));
}


/* three SHA-1 inner functions */
uint32_t ch(uint32_t x, uint32_t y, uint32_t z){
	DEBUG_S("\r\nCH");
	return ((x&y)^((~x)&z));
}

uint32_t maj(uint32_t x, uint32_t y, uint32_t z){
	DEBUG_S("\r\nMAJ");
	return ((x&y)^(x&z)^(y&z));
}

uint32_t parity(uint32_t x, uint32_t y, uint32_t z){
	DEBUG_S("\r\nPARITY");
	return ((x^y)^z);
}

/********************************************************************************************************/
/**
 * \brief "add" a block to the hash
 * This is the core function of the hash algorithm. To understand how it's working
 * and what thoese variables do, take a look at FIPS-182. This is an "alternativ" implementation 
 */

#define MASK 0x0000000f 

typedef uint32_t (*pf_t)(uint32_t x, uint32_t y, uint32_t z);

void sha1_nextBlock (sha1_ctx_t *state, void* block){
	uint32_t a[5];
	uint32_t w[16];
	uint32_t temp;
	uint8_t t,s;
	pf_t f[] = {ch,parity,maj,parity};
	uint32_t k[4]={	0x5a827999, 
					0x6ed9eba1, 
					0x8f1bbcdc, 
					0xca62c1d6};
	
	/* load the w array (changing the endian and so) */
	for(t=0; t<16; ++t){
		w[t] = change_endian32(((uint32_t*)block)[t]);
	}

	uint8_t dbgi;
	for(dbgi=0; dbgi<16; ++dbgi){
		DEBUG_S("\n\rBlock:");
		DEBUG_B(dbgi);
		DEBUG_C(':');
		#ifdef DEBUG
			uart_hexdump(&(w[dbgi]) ,4);
		#endif
	}
	
	
	/* load the state */
	memcpy(a, state->h, 5*sizeof(uint32_t));
	
	
	/* the fun stuff */
	for(t=0; t<=79; ++t){
		s = t & MASK;
		if(t>=16){
			#ifdef DEBUG
			 DEBUG_S("\r\n ws = "); uart_hexdump(&ws, 4);
			#endif
			w[s] = rotl32( w[(s+13)&MASK] ^ w[(s+8)&MASK] ^ 
				 w[(s+ 2)&MASK] ^ w[s] ,1);			
			#ifdef DEBUG
			 DEBUG_S(" --> ws = "); uart_hexdump(&(w[s]), 4);
			#endif
		}
		
		uint32_t dtemp;
		temp = rotl32(a[0],5) + (dtemp=f[t/20](a[1],a[2],a[3])) + a[4] + k[t/20] + w[s];
		memmove(&(a[1]), &(a[0]), 4*sizeof(uint32_t)); /* e=d; d=c; c=b; b=a; */
		a[0] = temp;
		a[2] = rotl32(a[2],30); /* we might also do rotr32(c,2) */
		
		/* debug dump */
		DEBUG_S("\r\nt = "); DEBUG_B(t);
		DEBUG_S("; a[]: ");
		#ifdef DEBUG
		 uart_hexdump(a, 5*4);
		#endif
		DEBUG_S("; k = ");
		#ifdef DEBUG
		 uart_hexdump(&(k[t/20]), 4);
		#endif
		DEBUG_S("; f(b,c,d) = ");
		#ifdef DEBUG
		 uart_hexdump(&dtemp, 4);
		#endif
	}
	
	/* update the state */
	for(t=0; t<5; ++t){
		state->h[t] += a[t];
	}
	state->length += 512;
}

/********************************************************************************************************/

void sha1_lastBlock(sha1_ctx_t *state, void* block, uint16_t length){
	uint8_t lb[SHA1_BLOCK_BITS/8]; /* local block */
	state->length += length;
	memcpy (&(lb[0]), block, length/8);
	
	/* set the final one bit */
	if (length & 0x7){ /* if we have single bits at the end */
		lb[length/8] = ((uint8_t*)(block))[length/8];
	} else {
		lb[length/8] = 0;
	}
	lb[length/8] |= 0x80>>(length & 0x3);
	length =(length >> 7) + 1; /* from now on length contains the number of BYTES in lb*/
	/* pad with zeros */
	if (length>64-8){ /* not enouth space for 64bit length value */
		memset((void*)(&(lb[length])), 0, 64-length);
		sha1_nextBlock(state, lb);
		state->length -= 512;
		length = 0;	
	}
	memset((void*)(&(lb[length])), 0, 56-length);
	/* store the 64bit length value */
#if defined LITTLE_ENDIAN
	 	/* this is now rolled up */
	uint8_t i; 	
	for (i=1; i<=8; ++i){
		lb[55+i] = (uint8_t)(state->length>>(64- 8*i));
	}
#elif defined BIG_ENDIAN
	*((uint64_t)&(lb[56])) = state->length;
#endif
	sha1_nextBlock(state, lb);
}

/********************************************************************************************************/

void sha1_ctx2hash (sha1_hash_t *dest, sha1_ctx_t *state){
#if defined LITTLE_ENDIAN
	uint8_t i;
	for(i=0; i<8; ++i){
		((uint32_t*)dest)[i] = change_endian32(state->h[i]);
	}
#elif BIG_ENDIAN
	if (dest != state->h)
		memcpy(dest, state->h, SHA256_HASH_BITS/8);
#else
# error unsupported endian type!
#endif
}

/********************************************************************************************************/
/**
 * 
 * 
 */
void sha1 (sha1_hash_t *dest, void* msg, uint32_t length){
	sha1_ctx_t s;
	DEBUG_S("\r\nBLA BLUB");
	sha1_init(&s);
	while(length & (~0x0001ff)){ /* length>=512 */
		DEBUG_S("\r\none block");
		sha1_nextBlock(&s, msg);
		msg = (uint8_t*)msg + SHA1_BLOCK_BITS/8; /* increment pointer to next block */
		length -= SHA1_BLOCK_BITS;
	}
	sha1_lastBlock(&s, msg, length);
	sha1_ctx2hash(dest, &s);
}


