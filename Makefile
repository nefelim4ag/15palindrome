15palindrome: main.c Makefile
	gcc -Wall -O3 main.c -o 15palindrome

primes_list.txt:
	primesieve 1 100 -p > primes_list.txt
