/* main-grain-test.c */
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
 * grain test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"
#include "cli.h"

#include "grain.h"
#include "nessie_stream_test.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "Grain";

/*****************************************************************************
 *  additional validation-functions                                          *
 *****************************************************************************/
void grain_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	uint8_t iv[8]={0};
	grain_init(key, &iv, ctx);
}

uint8_t grain_getbyte_dummy(grain_ctx_t* ctx){
	uint8_t i,ret=0;
	for(i=0; i<8; ++i){
		ret<<=1;
		ret |= grain_enc(ctx);
	}
	return ret;
}

uint8_t grain_getbyte_dummy_rev(grain_ctx_t* ctx){
	uint8_t i,ret=0;
	for(i=0; i<8; ++i){
		ret >>= 1;
		ret |= grain_enc(ctx)?0x80:0x00;
	}
	return ret;
}

void testrun_nessie_grain(void){
	nessie_stream_ctx.outsize_b =   8; /* actually unused */
	nessie_stream_ctx.keysize_b =  80; /* this is the one we have refrence vectors for */
	nessie_stream_ctx.ivsize_b  =  64;
	nessie_stream_ctx.name = algo_name;
	nessie_stream_ctx.ctx_size_B = sizeof(grain_ctx_t);
	nessie_stream_ctx.cipher_genctx = (nessie_stream_genctx_fpt)grain_genctx_dummy;
	nessie_stream_ctx.cipher_enc = (nessie_stream_genenc_fpt)grain_getbyte_dummy_rev;
	
	nessie_stream_run();	
}


void testrun_std_grain(void){
	grain_ctx_t ctx;
	uint8_t i, key[10], iv[8], out[10];
	
	/* 1 */
	memset(key, 0, 10);
	memset(iv, 0, 8);
	uart_putstr_P(PSTR("\r\n=== std test ==="));
	uart_putstr_P(PSTR("\r\n key: "));
	uart_hexdump(key, 10);
	uart_putstr_P(PSTR("\r\n iv:  "));
	uart_hexdump(key, 8);
	grain_init(key, iv, &ctx);
	for(i=0; i<10; ++i){
		out[i] = grain_getbyte_dummy(&ctx);
	}
	uart_putstr_P(PSTR("\r\n out: "));
	uart_hexdump(out, 10);
	
	/* 2 */
	for(i=0; i<8; ++i){
		key[i] = i*0x22+1;
	}
	key[8]=0x12;
	key[9]=0x34;
	
	for(i=0; i<8; ++i){
		iv[i] = i*0x22+1;
	}
	uart_putstr_P(PSTR("\r\n\r\n key: "));
	uart_hexdump(key, 10);
	uart_putstr_P(PSTR("\r\n iv:  "));
	uart_hexdump(key, 8);
	grain_init(key, iv, &ctx);
	for(i=0; i<10; ++i){
		out[i] = grain_getbyte_dummy(&ctx);
	}
	uart_putstr_P(PSTR("\r\n out: "));
	uart_hexdump(out, 10);
	
	
	uart_putstr_P(PSTR("\r\n\r\n"));
}

void testrun_performance_grain(void){
	uint64_t t;
	char str[16];
	uint8_t key[10], iv[8];
	grain_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();	
	
	memset(key,  0, 10);
	memset(iv,  0, 8);
	
	startTimer(1);
	grain_init(key, iv, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	startTimer(1);
	grain_enc(&ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);	
	
	uart_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main                                                                     *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_grain },
	{ test_str,        NULL, testrun_std_grain},
	{ performance_str, NULL, testrun_performance_grain},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	uart_putstr("\r\n");
	cli_rx = uart_getc;
	cli_tx = uart_putc;	 	
	for(;;){
		uart_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		uart_putstr(algo_name);
		uart_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
