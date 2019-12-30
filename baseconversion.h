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
        size_t ret = 0;

        for (int i = 0; src[i] != '\0' && src[i] != '\n'; ++i)
                ret = ret * 10 + __ctoi(src[i]);

        return ret;
}

static const char jump_table[36] = {
        '0','1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z'
};

static void base_num_to_36(char *dest, size_t num) {
        char *ptr_end = dest;

        while (num != 0) {
                int remainder = num % 36;
                *ptr_end = jump_table[remainder];
                ptr_end++;
                num = num / 36;
        }

        *ptr_end = '\0';

        strrev(dest);
}

static char* base_num_to_36_x(char *dest, size_t num) {
        while (num > 0) {
                *dest++ = jump_table[num % 36];
                num = num / 36;
        }

        return dest;
}

static char* r_base_num_to_36(char *dest, size_t num) {
        while (num > 0) {
                *dest-- = jump_table[num % 36];
                num = num / 36;
        }

        return dest;
}

static char* base_num_to_36_r(char *dest, size_t num) {
        size_t z = num / 36;
        size_t y = num % 36;

        if (z > 0) {
                dest = base_num_to_36_r(dest, z);
        }

        *dest = jump_table[y];

        return dest + 1;
}

static void base_10_to_36(char *dest, char *src) {
        char *ptr_end = dest;
        size_t num = fast_strtol(src);

        while (num != 0) {
                int remainder = num % 36;
                *ptr_end = jump_table[remainder];
                ptr_end++;
                num = num / 36;
        }

        *ptr_end = '\0';

        strrev(dest);
}
