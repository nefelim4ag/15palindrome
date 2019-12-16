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

static int is_palindrome(char *palindrome) {
        char *start_ptr = palindrome;
        char *end_ptr = palindrome + TGT_SRCH_SIZE - 1;

        while (start_ptr < end_ptr) {
                if (*start_ptr != *end_ptr)
                        return 0;
                start_ptr++;
                end_ptr--;
        }

        return 1;
}

static char *append_and_search(char *palindrome, char *num, size_t *counter) {
        size_t numlen = strlen(num);

        for (size_t i = 0; i < BUF_SIZE; i++) // Shift array by numlen
                palindrome[i] = palindrome[i + numlen];

        memcpy(palindrome + BUF_SIZE, num, numlen);

        while (*(palindrome + BUF_SIZE) != '\0') {
                *counter = *counter + 1;

                if (is_palindrome(palindrome))
                        return palindrome;

                palindrome++;
        }

        return NULL;
}

int main(int argc, char **argv) {
        char *ptr = NULL;
        char *ptr_end = NULL;
        char *success = NULL;
        clock_t begin, end;
        static char palindrome[BUF_SIZE * 2]; // Extra space for insert whole number
        size_t palindrome_offset = 0 - BUF_SIZE + 1; // for correct offset

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
                success[TGT_SRCH_SIZE] = '\0';
                printf("\n");
                printf("Palindrome found %s at byte %lu\n", success, palindrome_offset);
        } else {
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
