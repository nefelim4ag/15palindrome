#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define TGT_SRCH_SIZE 15
#define BUF_SIZE TGT_SRCH_SIZE + 1

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

static void strrev(char *str) {
        char *p1, *p2;

        if (! str || ! *str)
                return;
        for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
                *p1 ^= *p2;
                *p2 ^= *p1;
                *p1 ^= *p2;
        }
}

static void base_10_to_36(char *dest, char *src) {
        static const char jump_table[36] = {
                '0','1','2','3','4','5','6','7','8','9',
                'A','B','C','D','E','F','G','H','I','J',
                'K','L','M','N','O','P','Q','R','S','T',
                'U','V','W','X','Y','Z'
        };

        char *ptr_end = dest;
        size_t remainder;
        size_t num = strtol(src, NULL, 0);

        while (num > 36) {
                remainder = num % 36;
                *ptr_end = jump_table[remainder];
                ptr_end++;
                num = num / 36;
        }

        remainder = num % 36;
        *ptr_end = jump_table[remainder];

        strrev(dest);
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

static int append_and_search(char *palindrome, char *number_hex, size_t *counter) {
        while (*number_hex != '\0') {
                for (int i = 0; i < BUF_SIZE - 1; i++)
                        palindrome[i] = palindrome[i+1];
                palindrome[BUF_SIZE - 1] = *number_hex;
                // printf("%c", number_hex[0]);
                number_hex++;
                *counter = *counter + 1;
                if (is_palindrome(palindrome)) {
                        return 1;
                }
        }

        return 0;
}

int main(int argc, char **argv) {
        char *ptr = NULL;
        char *ptr_end = NULL;
        static char palindrome[BUF_SIZE + 1]; // static +1 for correct printing
        int success = 0;
        clock_t begin, end;

        size_t palindrome_offset = 0 - BUF_SIZE + 1;

        for (int i = 0; i < BUF_SIZE; i++)
                palindrome[i] = i;

        if (argc < 1) {
                printf("First arg must be file with prime numbers");
                return 1;
        }

        if (get_mmap(argv[1], &ptr, &ptr_end))
                return 1;

        begin = clock();
        while (ptr < ptr_end) {
                static char number_raw[BUF_SIZE];
                static char number_36[BUF_SIZE];
                char *ptr_num = number_raw;

                memset(&number_raw, '\0', sizeof(number_raw));
                memset(&number_36, '\0', sizeof(number_36));

                while (ptr < ptr_end && *ptr != '\n') {
                        *ptr_num = *ptr;
                        ptr_num++, ptr++;
                }

                base_10_to_36(number_36, number_raw);
                if (append_and_search(palindrome, number_36, &palindrome_offset)) {
                        printf("\n");
                        printf("Palindrome found %s at byte %lu\n", palindrome, palindrome_offset);
                        success = 1;
                        break;
                }

                ptr++;
        }
        end = clock();

        if (!success) {
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
