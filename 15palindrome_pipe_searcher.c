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

int main(int argc, char **argv) {
        static char rbuffer[64];
        static char palindrome[BUF_SIZE];
        static size_t palindrome_offset = 0;
        clock_t begin, end;
        int found = 0;

        FILE *fd = freopen(NULL, "rb", stdin);

        begin = clock();
        while (!found) {
                if (fread(&rbuffer, sizeof(rbuffer), 1, fd) < 0)
                        break;

                for (int a = 0; a < sizeof(rbuffer); a++) {
                        if (rbuffer[a] == '\n')
                                continue;

                        palindrome[palindrome_offset % TGT_SRCH_SIZE] = rbuffer[a];

                        if (is_palindrome(palindrome, palindrome_offset)) {
                                found = 1;
                                break;
                        }
                        palindrome_offset++;
                }
        }
        end = clock();



        if (is_palindrome(palindrome, palindrome_offset)) {
                printf("\n");
                printf("Palindrome found ");
                for (size_t i = 0; i < TGT_SRCH_SIZE; i++) {
                        printf("%c", palindrome[(palindrome_offset + i + 1) % TGT_SRCH_SIZE]);
                }
                printf( " at byte %lu\n", palindrome_offset);
        } else {
                fprintf(stderr, "Can't find palindrome(%i) in stdin\n", TGT_SRCH_SIZE);
        }

        fprintf(stderr, "Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
