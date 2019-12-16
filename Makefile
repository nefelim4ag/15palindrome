all: 15palindrome 15palindrome_pipe_10_to_36 15palindrome_pipe_searcher 15palindrome_mmap_searcher

15palindrome: 15palindrome.c Makefile baseconversion.h
	gcc -Wall -O3 15palindrome.c -o 15palindrome

15palindrome_pipe_10_to_36: 15palindrome_pipe_10_to_36.c Makefile baseconversion.h stdin.h
	gcc -Wall -O3 15palindrome_pipe_10_to_36.c -o 15palindrome_pipe_10_to_36

15palindrome_pipe_searcher: 15palindrome_pipe_searcher.c Makefile strdlen.h stdin.h
	gcc -Wall -O2 15palindrome_pipe_searcher.c -o 15palindrome_pipe_searcher

15palindrome_mmap_searcher: 15palindrome_mmap_searcher.c Makefile
	gcc -Wall -O3 15palindrome_mmap_searcher.c -o 15palindrome_mmap_searcher

baseconversion.h: strdlen.h
