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

#include "strdlen.h"

#define TGT_SRCH_SIZE 15
#define BUF_SIZE TGT_SRCH_SIZE

static int is_palindrome(char *palindrome) {
        char *ptr_start  = palindrome;
        char *ptr_end = palindrome + TGT_SRCH_SIZE - 1;
#if TGT_SRCH_SIZE >= 15
{
        uint64_t *ptr_start = (uint64_t *) palindrome;
        uint64_t *ptr_end = (uint64_t *) (palindrome + TGT_SRCH_SIZE - sizeof(uint64_t));
        if (*ptr_start != bswap_64(*ptr_end))
                return 0;
}
#elif TGT_SRCH_SIZE >= 7
{
        uint32_t *ptr_start = (uint32_t *) palindrome;
        uint32_t *ptr_end = (uint32_t *) (palindrome + TGT_SRCH_SIZE - sizeof(uint32_t));
        if (*ptr_start != bswap_32(*ptr_end))
                return 0;
}
#endif

        while (ptr_start < ptr_end) {
                if (*ptr_start != *ptr_end)
                        return 0;
                ptr_start++;
                ptr_end--;
        }

        return 1;
}

static char *append_and_search(char *palindrome, char *num, size_t *counter) {
        size_t numlen = strdlen(num);

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

struct stdin_read_buffer {
        FILE *stdin;
        char buffer[64 * 1024];
        char last_line[128];
        size_t read_chunk;
        size_t bytes_read;
        size_t last_line_size;
        char *ptr;
        char *ptr_last_line;
        char *ptr_end;
};

static struct stdin_read_buffer B = {
        .read_chunk = 512,
        .stdin = NULL
};

void fix_string(char *a) {
        for(; *a != '\n' && *a != '\0'; a++);
        *a = '\0';
}

char *get_stdin_line(){
        if (B.stdin == NULL)
                B.stdin = freopen(NULL, "rb", stdin);

        if (B.ptr < B.ptr_last_line) {
                while (*B.ptr != '\n' && *B.ptr != '\0')
                        B.ptr++;

                while (*B.ptr == '\n' && B.ptr < B.ptr_last_line) {
                        B.ptr++;
                }

                while (*B.ptr == '\0' && B.ptr < B.ptr_last_line) {
                        B.ptr++;
                }

                //B.ptr++;

                if (B.ptr >= B.ptr_last_line) {
                        char *t = B.last_line;
                        if (B.stdin == NULL)
                                return NULL;
                        return t;
                }
        } else {
                size_t read_count = sizeof(B.buffer)/B.read_chunk;
                memset(B.buffer, '\0', sizeof(B.buffer));
                B.bytes_read = fread(B.buffer, B.read_chunk, read_count, B.stdin);
                B.ptr = B.buffer;
                B.ptr_end = B.buffer + sizeof(B.buffer) - 1;
                B.ptr_last_line = B.ptr_end;

                if (B.bytes_read < 0)
                        return NULL;

                while (*B.ptr_last_line != '\n' && *B.ptr_last_line != '\0') {
                        B.ptr_last_line--;
                }

                if (*B.ptr_last_line == '\n')
                        *B.ptr_last_line = '\0';

                B.last_line_size = B.ptr_end - B.ptr_last_line;
                memcpy(B.last_line, B.ptr_last_line + 1, B.last_line_size);

                char c;
                do {
                        c = fgetc(B.stdin);
                        if (c < 0) {
                                fclose(B.stdin);
                                B.stdin = NULL;
                                B.last_line[B.last_line_size] = '\0';
                                break;
                        }
                        if (c == '\n')
                                break;
                        B.last_line[B.last_line_size] = c;
                        B.last_line_size++;
                } while (c != '\n' && c != '\0');

                B.last_line[B.last_line_size] = '\0';
        }

        return B.ptr;
}

int main(int argc, char **argv) {
        static char palindrome[BUF_SIZE * 2]; // Extra space for insert whole number
        char *success = NULL;
        clock_t begin, end;
        size_t palindrome_offset = 0 - BUF_SIZE + 1; // for correct offset

        for (int i = 0; i < TGT_SRCH_SIZE; i++)
                palindrome[i] = 48 + i;


        begin = clock();
        while (!success) {
                char *ptr = get_stdin_line();
                if (ptr == NULL)
                        break;
                //fix_string(ptr);
                // printf("%s\n", ptr);
                success = append_and_search(palindrome, ptr, &palindrome_offset);
        }
        end = clock();

        if (success) {
                success[TGT_SRCH_SIZE] = '\0';
                printf("\n");
                printf("Palindrome found %s at byte %lu\n", success, palindrome_offset);
        } else {
                fprintf(stderr, "Can't find palindrome(%i) in stdin\n", TGT_SRCH_SIZE);
        }

        fprintf(stderr, "Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
