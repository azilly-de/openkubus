/* main-des-test.c */
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
 * des test-suit
 * 
*/

#include "config.h"
#include "serial-tools.h"
#include "uart.h"
#include "debug.h"

#include "des.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "DES";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void des_init_dummy(const void* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, 8);
}

void des_enc_dummy(void* buffer, void* ctx){
	des_enc(buffer, buffer, ctx);
} 

void des_dec_dummy(void* buffer, void* ctx){
	des_dec(buffer, buffer, ctx);
} 

void testrun_nessie_des(void){
	nessie_bc_init();
	nessie_bc_ctx.blocksize_B =   8;
	nessie_bc_ctx.keysize_b   =  64;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = 8;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)des_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)des_dec_dummy;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)des_init_dummy;
	
	nessie_bc_run();
}


void testrun_performance_des(void){
	uint64_t t;
	char str[16];
	uint8_t key[8], data[8];
	
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 8);
	memset(data, 0, 8);
	
	startTimer(1);
	des_enc(data, data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	
	startTimer(1);
	des_dec(data, data, key);
	t = stopTimer();
	uart_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	uart_putstr(str);
	uart_putstr_P(PSTR("\r\n"));
}
/*****************************************************************************
 * main						
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_des },
	{ test_str,        NULL, testrun_nessie_des },
	{ performance_str, NULL, testrun_performance_des},
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
