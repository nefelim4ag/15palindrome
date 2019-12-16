#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 32

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

static inline uint8_t __ctoi(char num) {
        return num - 48;
}

static size_t fast_strtol(char *src) {
        char *ptr = src;
        size_t len;
        size_t ret = 0;
        size_t base = 1;

        while (*ptr != '\n' && *ptr != '\0') {
                ptr++;
        }

        len = ptr - src;

        for (size_t i = len - 1; i > 0; i--)
                base *= 10;

        for (size_t i = 0; i < len; i++) {
                ret += __ctoi(src[i]) * base;
                base /= 10;
        }

        return ret;
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
        size_t num = fast_strtol(src);

        while (num > 36) {
                remainder = num % 36;
                *ptr_end = jump_table[remainder];
                ptr_end++;
                num = num / 36;
        }

        remainder = num % 36;
        *ptr_end = jump_table[remainder];

        if (num == 36) {
                ptr_end++;
                *ptr_end = jump_table[num / 36];
        }

        ptr_end++;
        *ptr_end = '\0';

        strrev(dest);
}

int main(int argc, char **argv) {
        static size_t buf_size = BUF_SIZE;
        static char input_buf[BUF_SIZE];
        static char output_buf[BUF_SIZE];
        static char *ptr = input_buf;

        while (getline(&ptr, &buf_size, stdin) > 0) {
                base_10_to_36(output_buf, input_buf);
                printf("%s\n", output_buf);
        }
        return 0;
}
