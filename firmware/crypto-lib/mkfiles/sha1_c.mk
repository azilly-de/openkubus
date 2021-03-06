# Makefile for SHA1
ALGO_NAME := SHA1_C

# comment out the following line for removement of SHA1 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := sha1.o
$(ALGO_NAME)_TEST_BIN := main-sha1-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o string-extras.o \
			 performance_test.o hfal-basic.o hfal_sha1.o shavs.o dump.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

