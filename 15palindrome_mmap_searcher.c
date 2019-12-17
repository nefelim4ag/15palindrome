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
        *ptr = (char*) mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);

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

int main(int argc, char **argv) {
        int success = 0;
        clock_t begin, end;
        char *ptr = NULL;
        char *ptr_start = NULL;
        char *ptr_end = NULL;

        if (argc < 1) {
                fprintf(stderr, "First arg must be file with prime numbers");
                return 1;
        }

        if (get_mmap(argv[1], &ptr, &ptr_end))
                return 1;
        ptr_start = ptr;
        begin = clock();
        while (ptr < ptr_end) {
                success = is_palindrome(ptr);
                ptr++;
                if (success) {
                        printf("\n");
                        printf("Palindrome found %s at byte %lu\n", "test", ptr - ptr_start);
                        break;
                }
        }
        end = clock();

        if (!success)
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
