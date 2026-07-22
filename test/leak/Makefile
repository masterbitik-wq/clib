CC ?= gcc
SAN_CFLAGS = -fsanitize=address -g -O0 -fno-omit-frame-pointer
SAN_LDFLAGS = -fsanitize=address

SRC = ../../src/common/clib-package.c ../../src/common/clib-cache.c
DEPS = $(wildcard ../../deps/*/*.c)
OBJS_COMMON = $(SRC:.c=.o) $(filter-out ../../deps/asprintf/asprintf.c,$(DEPS:.c=.o))
OBJS_SRC = $(SRC:.c=.o) $(DEPS:.c=.o)

CFLAGS += -std=c99 -Wall -I../../src/common -I../../deps -DHAVE_PTHREADS -pthread -g
LDFLAGS = -lcurl

.DEFAULT_GOAL := all

all: flags-loop-repro package-leak-src

flags-loop-repro: flags-loop-repro.c
	$(CC) $(SAN_CFLAGS) $^ -o $@ $(SAN_LDFLAGS)

package-leak-src: package-leak-src.o fail_list_node.o $(OBJS_SRC)
	$(CC) $(SAN_CFLAGS) $^ -o $@ $(LDFLAGS) $(SAN_LDFLAGS) -Wl,--wrap=list_node_new

clean:
	rm -f $(OBJS_SRC) *.o flags-loop-repro package-leak-src package-leak-flags

.PHONY: all clean
