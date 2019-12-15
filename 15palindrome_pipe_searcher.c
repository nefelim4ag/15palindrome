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
        while (*number_hex != '\0' && *number_hex != '\n' ) {
                for (int i = 0; i < BUF_SIZE - 1; i++) //gcc not like overlaps in memcpy
                        palindrome[i] = palindrome[i+1];

                palindrome[BUF_SIZE - 1] = *number_hex;
                number_hex++;
                *counter = *counter + 1;

                if (is_palindrome(palindrome))
                        return 1;
        }

        return 0;
}

int main(int argc, char **argv) {
        static char palindrome[BUF_SIZE + 1]; // static +1 for correct printing
        int ret = 1;
        int success = 0;
        clock_t begin, end;
        size_t palindrome_offset = 0 - BUF_SIZE + 1; // for correct offset

        for (int i = 0; i < BUF_SIZE; i++)
                palindrome[i] = i;

        static size_t buf_size = BUF_SIZE;
        static char input_buf[BUF_SIZE];
        static char *ptr = input_buf;



        begin = clock();
        while (ret > 0) {
                getline(&ptr, &buf_size, stdin);

                success = append_and_search(palindrome, input_buf, &palindrome_offset);
                if (success) {
                        printf("\n");
                        printf("Palindrome found %s at byte %lu\n", palindrome, palindrome_offset);
                        break;
                }
        }
        end = clock();

        if (!success)
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
