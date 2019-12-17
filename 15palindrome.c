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

#include "baseconversion.h"

#define TGT_SRCH_SIZE 15
#define BUF_SIZE TGT_SRCH_SIZE

static int get_mmap(char *path, char **ptr, char **ptr_end) {
        int fd = open(path, O_RDONLY);
        size_t len;
        static struct stat buf;

        if (fd < 0) {
                fprintf(stderr,"Error: Unable to read dictionary file %s\n", path);
                return -1;
        }

        if (fstat(fd, &buf) < 0) {
                fprintf(stderr,"Error: Unable to determine file size\n");
                return -1;
        }

        len = (uint64_t) buf.st_size;
        *ptr = (char*) mmap(0, len, PROT_READ, MAP_POPULATE | MAP_PRIVATE, fd, 0);

        if (ptr == MAP_FAILED) {
                fprintf(stderr,"Error: Unable to memory map dictionary!\n");
                return -1;
        }

        *ptr_end = *ptr + len;

        return 0;
}

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
        char *ptr = NULL;
        char *ptr_end = NULL;
        int success = 0;
        clock_t begin, end;
        static char palindrome[BUF_SIZE];
        size_t palindrome_offset = 0; // for correct offset

        for (size_t i = 0; i < TGT_SRCH_SIZE; i++)
                palindrome[i] = 48 + i;

        if (argc < 1) {
                fprintf(stderr, "First arg must be file with prime numbers");
                return 1;
        }

        if (get_mmap(argv[1], &ptr, &ptr_end))
                return 1;

        begin = clock();
        while (ptr < ptr_end && !success) {
                static char num[BUF_SIZE];
                char *ptr_curr_num = ptr;

                base_10_to_36(num, ptr_curr_num);
                success = append_and_search(palindrome, num, &palindrome_offset);

                while (ptr < ptr_end && *ptr != '\n')
                        ptr++;
                ptr++;
        }
        end = clock();

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
