all: 15palindrome 15palindrome_pipe_10_to_36 15palindrome_pipe_searcher 15palindrome_mmap_searcher 15palindrome_amazing_fast

15palindrome: 15palindrome.c Makefile baseconversion.h
	gcc -Wall -O3 15palindrome.c -o 15palindrome

15palindrome_pipe_10_to_36: 15palindrome_pipe_10_to_36.c Makefile baseconversion.h stdin.h
	gcc -Wall -O3 15palindrome_pipe_10_to_36.c -o 15palindrome_pipe_10_to_36

15palindrome_pipe_searcher: 15palindrome_pipe_searcher.c Makefile strdlen.h stdin.h
	gcc -Wall -O3 15palindrome_pipe_searcher.c -o 15palindrome_pipe_searcher

15palindrome_mmap_searcher: 15palindrome_mmap_searcher.c Makefile
	gcc -Wall -O3 -g 15palindrome_mmap_searcher.c -o 15palindrome_mmap_searcher

15palindrome_amazing_fast: 15palindrome_amazing_fast.c Makefile
	gcc -Wall -O3 $@.c -o $@ -lprimesieve

baseconversion.h: strdlen.h


.PHONY: clean
clean:
	rm -f 15palindrome_mmap_searcher 15palindrome_pipe_searcher 15palindrome_pipe_10_to_36 15palindrome
