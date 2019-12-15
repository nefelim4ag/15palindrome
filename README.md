Let's convert prime numbers to base 36 and concat it in long string

Let's find palindrome with defined size (15 in opener) in that string

For generate Prime numbers, let's use `primesieve`

Example of usage:
```
➜  ~ primesieve 1 100000000 -p > primes_list.txt
➜  15palindrome (master) ~ make
➜  15palindrome (master) ~ ./15palindrome ~/primes_list.txt
```

```
➜  15palindrome git:(master) ✗ make all; cat ~/primes_list.txt | ./15palindrome_pipe_10_to_36 | ./15palindrome_pipe_searcher
Palindrome found Z11AA11N11AA11Z1 at byte 657421207
Time spend: 7.382601 sec
```
