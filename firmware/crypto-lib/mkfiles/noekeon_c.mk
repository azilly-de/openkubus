# Makefile for noekeon
ALGO_NAME := NOEKEON_C

# comment out the following line for removement of noekeon from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := noekeon.o
$(ALGO_NAME)_TEST_BIN := main-noekeon-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_bc_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

