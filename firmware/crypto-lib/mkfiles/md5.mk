# Makefile for MD5
ALGO_NAME := MD5

# comment out the following line for removement of MD5 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := md5.o
$(ALGO_NAME)_TEST_BIN := main-md5-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

