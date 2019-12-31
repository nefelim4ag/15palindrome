
APPS = 15palindrome 15palindrome_pipe_10_to_36 15palindrome_pipe_searcher 15palindrome_mmap_searcher 15palindrome_amazing_fast 15palindrome_amazing_fast_multithread

CC = gcc
CFLAGS = -Wall -O3 -g
LDFLAGS = -lprimesieve -lpthread 

DEPS = Makefile baseconversion.h stdin.h

all: ${APPS}

${APPS}: %: %.c ${DEPS}
	${CC} ${CFLAGS} $@.c -o $@ ${LDFLAGS}

baseconversion.h: strdlen.h


.PHONY: clean
clean:
	rm -f ${APPS}