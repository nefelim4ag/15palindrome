#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "strdlen.h"

static void strrev(char *str) {
        char *p1, *p2;

        if (! str || ! *str)
                return;
        for (p1 = str, p2 = str + strdlen(str) - 1; p2 > p1; ++p1, --p2) {
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

static const char jump_table[36] = {
        '0','1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z'
};

void base_10_to_36(char *dest, char *src) {
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
                *ptr_end = '1';
        }

        ptr_end++;
        *ptr_end = '\0';

        strrev(dest);
}
