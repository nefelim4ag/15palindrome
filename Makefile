15palindrome: main.c
	gcc -Wall -O3 main.c -o 15palindrome

primes_list.txt:
	primesieve 1 100 -p > primes_list.txt
