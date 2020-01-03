
APPS = 15palindrome 15palindrome_pipe_10_to_36 15palindrome_pipe_searcher 15palindrome_mmap_searcher 15palindrome_amazing_fast
APPS += 15palindrome_amazing_fast_multithread 15palindrome_amazing_fast_multithread_sync

CC = gcc
CFLAGS = -Wall -O3 -gdwarf-2 -D_GNU_SOURCE
LDFLAGS = -lprimesieve -lpthread 

DEPS = Makefile baseconversion.h stdin.h 15palindrome.h

all: ${APPS}

${APPS}: %: %.c ${DEPS}
	${CC} ${CFLAGS} $@.c -o $@ ${LDFLAGS}
	objdump -S $@ > $@.S

baseconversion.h: strdlen.h


.PHONY: clean
clean:
	rm -f ${APPS}