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
#include <primesieve.h>


#define TGT_SRCH_SIZE 15
#define BUF_SIZE TGT_SRCH_SIZE

static const char jump_table[36] = {
        '0','1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z'
};

static char* base_num_to_36_r(char *dest, size_t num) {
        size_t z = num / 36;
        size_t y = num % 36;

        if (z > 0) {
                dest = base_num_to_36_r(dest, z);
        }

        *dest = jump_table[y];

        return dest + 1;
}

static char* base_num_to_36(char *dest, size_t num) {
        static char buffer[16];
        char* ptr = buffer;
        while (num > 0) {
                *ptr = jump_table[num % 36];
                ptr++;
                num = num / 36;
        }
        while (ptr > buffer) {
                ptr--;
                *dest = *ptr;
                dest++;
        }
        return dest;
}

inline int is_palindrome_15(char *ptr) {
        return ptr[0] == ptr[14]
            && ptr[1] == ptr[13]
            && ptr[2] == ptr[12]
            && ptr[3] == ptr[11]
            && ptr[4] == ptr[10]
            && ptr[5] == ptr[9]
            && ptr[6] == ptr[8];
}

inline int is_palindrome_15_bswap(char *ptr) {
        uint64_t* first = (uint64_t*) ptr;

        uint64_t* second = (uint64_t*) (ptr + 7);

        return bswap_64(*first) == *second;
}


int search(char* begin, char* end, size_t *palindrome_offset) {
        char* ptr = begin;
        while (ptr <= end - 15) {
                if (is_palindrome_15_bswap(ptr)) {
                        *palindrome_offset = ptr - begin;
                        return 1;
                }
                ptr++;
        }
        *palindrome_offset = ptr - begin;
        return 0;
}

void test_base36(size_t x) {
        char b1[16] = {0};
        char b2[16] = {0};
        char* r1 = base_num_to_36(b1, x);
        char* r2 = base_num_to_36_r(b2, x);
        if ((r1 - b1) != (r2 - b2) || memcmp(b1, b2, r2 - b2) != 0) {
                printf("fail %lu\nb1 %lu: ", x, r1 - b1);
                for (size_t i = 0; i < r1 - b1; i++) {
                        printf("%c", b1[i]);
                }
                printf("\nb2 %lu: ", r2 - b2);
                for (size_t i = 0; i < r2 - b2; i++) {
                        printf("%c", b2[i]);
                }
                printf("\n");
                exit(1);
        }
}

int main(int argc, char **argv) {
        int success = 0;
        clock_t begin, end;
        size_t buffer_offset = 0; // for correct offset
        size_t total_offset = 0; // for correct offset

        primesieve_iterator it;
        primesieve_init(&it);
        size_t prime;
        static char base36_buffer[4096*16];
        char* ptr = base36_buffer;

        begin = clock();
        while(1) {
                while(ptr < (base36_buffer + sizeof(base36_buffer) - 16)) {
                        prime = primesieve_next_prime(&it);
                        //test_base36(prime);
                        ptr = base_num_to_36(ptr, prime);
                }
                if (search(base36_buffer, ptr, &buffer_offset)) {
                        total_offset += buffer_offset;
                        success = 1;
                        break;
                }
                total_offset += buffer_offset;
                memcpy(base36_buffer, ptr-14, 14);
                ptr = base36_buffer + 14;
        }
        end = clock();

        primesieve_free_iterator(&it);

        if (success) {
                printf("\n");
                printf("Palindrome found ");
                for (size_t i = 0; i < TGT_SRCH_SIZE; i++) {
                        printf("%c", base36_buffer[buffer_offset + i]);
                }
                printf( " at byte %lu\n", total_offset);
        } else {
                printf("Can't find palindrome(%i) in %s\n", TGT_SRCH_SIZE, argv[1]);
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);

        //while(1);
}
