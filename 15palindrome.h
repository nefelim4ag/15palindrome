#include <unistd.h>
#include <byteswap.h>
#include <primesieve.h>

const char jump_table[36] = {
        '0','1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G','H','I','J',
        'K','L','M','N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z'
};

static inline uint64_t fastmod36(uint64_t n)
{
        const uint64_t c = UINT64_C(0xFFFFFFFFFFFFFFFF) / 36 + 1;
        register uint64_t lowbits = c * n;
        return ((__uint128_t)lowbits * 36) >> 64; 
}

char* base36_r(char *dest, uint64_t num) {
        uint64_t y = num % 36;
        //uint64_t y = fastmod36(num);
        uint64_t z = num / 36;
        if (z > 0) {
                dest = base36_r(dest, z);
        }
        // NOT tail-recursive, but faster than base36 if apply to batch
        *dest = jump_table[y];
        return dest + 1;
}

#if 0
char* base36(char *dest, size_t num) {
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
#endif

int is_palindrome_15_bswap(const char *ptr) {
        uint64_t* first = (uint64_t*) ptr;

        uint64_t* second = (uint64_t*) (ptr + 7);

        return bswap_64(*first) == *second;
}


ssize_t search_15_palindrome(const char* begin, const char* end) {
        const char* ptr = begin;
        while (ptr <= end - 15) {
                if (is_palindrome_15_bswap(ptr)) {
                        return ptr - begin;
                }
                ptr++;
        }
        return -1;
}


void prime_fill_buffer(uint64_t* buffer, size_t size, primesieve_iterator* it)
{
        while (size-- > 0) {
                *buffer++ = primesieve_next_prime(it);
        }
}

char* base36_print_buffer(char* dest, const uint64_t* src, size_t size)
{
        while(size-- > 0) {
                dest = base36_r(dest, *src++);
        }
        return dest;
}

size_t base36_print_buffer_2(char* dest, const uint64_t* src, size_t size)
{
        char* dest_begin = dest;
        while(size-- > 0) {
                dest = base36_r(dest, *src++);
        }
        return dest - dest_begin;
}

