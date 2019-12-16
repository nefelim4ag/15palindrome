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
#define BUF_SIZE TGT_SRCH_SIZE

static int is_palindrome(char *palindrome) {
        char *ptr_start  = palindrome;
        char *ptr_end = palindrome + TGT_SRCH_SIZE - 1;

        while (ptr_start < ptr_end) {
                if (*ptr_start != *ptr_end)
                        return 0;
                ptr_start++;
                ptr_end--;
        }

        return 1;
}

static char *append_and_search(char *palindrome, char *num, size_t *counter) {
        size_t numlen = strlen(num) - 1; // -1 Ignore new line char

        for (size_t i = 0; i < BUF_SIZE; i++) // Shift array by numlen
                palindrome[i] = palindrome[i + numlen];

        memcpy(palindrome + BUF_SIZE, num, numlen);

        while(*(palindrome + BUF_SIZE) != '\0') {
                *counter = *counter + 1;

                if (is_palindrome(palindrome))
                        return palindrome;

                palindrome++;
        }

        return NULL;
}

int main(int argc, char **argv) {
        static char palindrome[BUF_SIZE * 2]; // Extra space for insert whole number
        char *success = NULL;
        clock_t begin, end;
        size_t palindrome_offset = 0 - BUF_SIZE + 1; // for correct offset

        for (int i = 0; i < TGT_SRCH_SIZE; i++)
                palindrome[i] = 48 + i;

        static size_t buf_size = BUF_SIZE;
        static char input_buf[BUF_SIZE];
        static char *ptr = input_buf;

        begin = clock();
        while (getline(&ptr, &buf_size, stdin) > 0 && !success) {
                success = append_and_search(palindrome, input_buf, &palindrome_offset);
        }
        end = clock();

        if (success) {
                success[TGT_SRCH_SIZE] = '\0';
                printf("\n");
                printf("Palindrome found %s at byte %lu\n", success, palindrome_offset);
        } else {
                printf("Can't find palindrome(%i) in stdin\n", TGT_SRCH_SIZE);
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
