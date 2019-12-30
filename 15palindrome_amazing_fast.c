#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <byteswap.h>
#include <primesieve.h>

#include "baseconversion.h"

#define TGT_SRCH_SIZE 15
#define BUF_SIZE TGT_SRCH_SIZE

static inline int is_palindrome(char *palindrome, size_t offset) {
        for (int i = 0; i < TGT_SRCH_SIZE / 2; i++) {
                size_t last_byte = (offset - i) % TGT_SRCH_SIZE;
                size_t first_byte = (offset + i + 1) % TGT_SRCH_SIZE;

                if (palindrome[last_byte] != palindrome[first_byte])
                        return 0;
        }

        return 1;
}

static int append_and_search(char *palindrome, char *rbuffer, size_t *palindrome_offset) {
        while (*rbuffer != '\0') {
                palindrome[*palindrome_offset % TGT_SRCH_SIZE] = *rbuffer;
                rbuffer++;

                if (is_palindrome(palindrome, *palindrome_offset)) {
                        return 1;
                }
                *palindrome_offset = *palindrome_offset + 1;
        }

        return 0;
}

int main(int argc, char **argv) {
        int success = 0;
        clock_t begin, end;
        static char palindrome[BUF_SIZE + 1 ];
        size_t palindrome_offset = 0; // for correct offset

        primesieve_iterator it;
        primesieve_init(&it);
        size_t prime;

        /* Iterate over the primes below 10^6 */
        begin = clock();
        while ((prime = primesieve_next_prime(&it)) < (size_t) 5 * 1000 * 1000 * 1000) {
                static char num[BUF_SIZE];

                base_num_to_36(num, prime);
                success = append_and_search(palindrome, num, &palindrome_offset);
                if (success)
                        break;
        }
        end = clock();

        primesieve_free_iterator(&it);

        if (success) {
                printf("\n");
                printf("Palindrome found ");
                for (size_t i = 0; i < TGT_SRCH_SIZE; i++) {
                        printf("%c", palindrome[(palindrome_offset + i + 1) % TGT_SRCH_SIZE]);
                }
                printf( " at byte %lu\n", palindrome_offset);
        } else {
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
