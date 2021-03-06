/* main-aes192-test.c */
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
 * AES-192 test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "aes.h"
#include "aes192_enc.h"
#include "aes192_dec.h"
#include "aes_keyschedule.h"

#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "AES-192";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_aes(void){
	nessie_bc_ctx.blocksize_B =  16;
	nessie_bc_ctx.keysize_b   = 192;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = sizeof(aes192_ctx_t);
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)aes192_enc;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)aes192_dec;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)aes_init;
	nessie_bc_run();
}

void testrun_testkey_aes192(void){
	uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7, 
	                    0xda, 0x0e, 0x64, 0x52,
	                    0xc8, 0x10, 0xf3, 0x2b, 
	                    0x80, 0x90, 0x79, 0xe5, 
	                    0x62, 0xf8, 0xea, 0xd2, 
	                    0x52, 0x2c, 0x6b, 0x7b};
	aes192_ctx_t ctx;
	uint8_t i;
	memset(&ctx, 0, sizeof(aes192_ctx_t));
	aes192_init(key, &ctx);
	uart_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	uart_hexdump(key, 24);
	for(i=0; i<13; ++i){
		uart_putstr_P(PSTR("\r\n index: "));
		uart_putc('0'+i/10);
		uart_putc('0'+i%10);
		uart_putstr_P(PSTR(" roundkey "));
		uart_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes(void){
	testrun_testkey_aes192();
}
/*****************************************************************************/

void testrun_performance_aes192(void){
	uint64_t t;
	char str[16];
	uint8_t key[32], data[16];
	aes192_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 32);
	memset(data, 0, 16);
	
	startTimer(1);
	aes192_init(key, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	aes192_enc(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	
	startTimer(1);
	aes192_dec(data, &ctx);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	uart_putstr_P(PSTR("\r\n"));
}

void testrun_performance_aes(void){
	uart_putstr_P(PSTR("\r\n -=AES Performance Test=-\r\n"));
	uart_putstr_P(PSTR("\r\n       AES-192\r\n"));
	testrun_performance_aes192();
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testkey_str[]     PROGMEM = "testkey";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_aes },
	{ test_str,        NULL, testrun_nessie_aes},
	{ testkey_str,     NULL, testrun_testkey_aes},
	{ performance_str, NULL, testrun_performance_aes},
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

