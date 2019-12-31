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
#include <pthread.h>

#define ENABLE_TRACE 0
#define ENABLE_SW_MEMORY_BARRIER 1
#define ENABLE_SLEEP 1
#define ENABLE_HW_MEMORY_BARRIER 0

#if ENABLE_TRACE
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...) 
#endif

#if ENABLE_SW_MEMORY_BARRIER
#define SW_MEM_BARRIER asm volatile("": : :"memory")
#else
#define SW_MEM_BARRIER
#endif

inline void spin_sleep()
{
        int _c = 1000;
        while (_c-- > 0) {
                (void)0;
        }
}

#if ENABLE_SLEEP
//#define SLEEP usleep(1)
//#define SLEEP {struct timespec tim, tim2; tim.tv_sec=0; tim.tv_nsec=1000; nanosleep(&tim, &tim2);}
#define SLEEP spin_sleep()
#else
#define SLEEP
#endif

#if ENABLE_HW_MEMORY_BARRIER
#define HW_MEM_BARRIER __sync_synchronize()
#else
#define HW_MEM_BARRIER
#endif


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

__attribute__((aligned(64))) int run = 1;

primesieve_iterator it;

#define PRIME_BUFFER_SIZE 1024*512

__attribute__((aligned(64))) uint64_t prime_buffer[2][PRIME_BUFFER_SIZE];
__attribute__((aligned(64))) volatile uint64_t* prime_buffer_w = 0;
__attribute__((aligned(64))) volatile const uint64_t* prime_buffer_r = 0;

//TODO: notify threads via conditional variables
//TODO: more smaller buffers and lockless queue
//TODO: synchronize with pthread_barrier_wait

void* prime_gen_routine(void* arg)
{
        size_t buffer_count = 0;
        size_t sleep_count = 0;
        uint64_t* local_prime_buffer_w = prime_buffer[1];

        while(run) {
                // do work
                prime_fill_buffer(local_prime_buffer_w, PRIME_BUFFER_SIZE, &it);
                TRACE("gen: filled\n");

                // publish work result for base36 thread
                prime_buffer_r = local_prime_buffer_w;
                SW_MEM_BARRIER;
                HW_MEM_BARRIER;
                TRACE("gen: published %p\n", prime_buffer_r);
                buffer_count++;

                // wait for base36 thread consumed previous batch
                while(prime_buffer_w == local_prime_buffer_w || prime_buffer_w == 0) {
                        SW_MEM_BARRIER;
                        if (!run) { break; }
                        SLEEP;
                        sleep_count++;
                }
                local_prime_buffer_w = (uint64_t*) prime_buffer_w;
                TRACE("gen: consumed %p\n", local_prime_buffer_w);
        }

        printf("gen: published %lu buffers, %lu bytes, %lu sleep calls\n",
                buffer_count, buffer_count*PRIME_BUFFER_SIZE*sizeof(*prime_buffer_w), sleep_count);
        return 0;
}

void switch_prime_buffer() {
        if (prime_buffer_w == prime_buffer[0]) {
                prime_buffer_w = prime_buffer[1];
        } else {
                prime_buffer_w = prime_buffer[0];
        }
        SW_MEM_BARRIER;
        HW_MEM_BARRIER;
}

// sizeof(uint64_t) * 256 / 36 < 64
__attribute__((aligned(64))) char base36_buffer[2][PRIME_BUFFER_SIZE * 64]; 
__attribute__((aligned(64))) volatile char* base36_buffer_w = 0;
__attribute__((aligned(64))) volatile const char* base36_buffer_r = 0;
__attribute__((aligned(64))) volatile const char* base36_buffer_r_end = 0;

void* base36_routine(void* arg)
{
        size_t byte_count = 0;
        size_t sleep_count = 0;

        TRACE("36: started\n");
        // wait for prime buffer ready first time
        while (prime_buffer_r == 0) {
                SW_MEM_BARRIER;
                //TRACE("36: prime_buffer_r=%p\n", prime_buffer_r);
                if (!run) { break; }
                SLEEP;
                sleep_count++;
        }
        TRACE("36: ready 1\n");

        char* local_base36_buffer_w = base36_buffer[1];
        char* write_ptr = local_base36_buffer_w;
        char tail[14] = {0};

        while(run) {
                // save prime read ptr
                const uint64_t* local_prime_buffer_r = (const uint64_t*) prime_buffer_r;
                // let generator proceed
                switch_prime_buffer();
                TRACE("36: switched %p\n", local_prime_buffer_r);

                if (base36_buffer_w != 0) {
                        // if search already switched base36 write buffer
                        // restore tail of previous buffer into head of next one
                        memcpy(write_ptr, tail, 14);
                        write_ptr += 14;
                }
                // do work
                base36_buffer_r_end = base36_print_buffer(write_ptr, local_prime_buffer_r, PRIME_BUFFER_SIZE);
                // store tail of output for future use
                // notice: double copy, first into temporary buffer then into output buffer
                //         works faster than one copy after wait for search thread consumed previous batch
                memcpy(tail, (const char*)base36_buffer_r_end - 14, 14);
                TRACE("36: done\n");

                // publish work result for search thread
                base36_buffer_r = local_base36_buffer_w;
                SW_MEM_BARRIER;
                HW_MEM_BARRIER;
                TRACE("36: published %p\n", base36_buffer_r);
                byte_count += base36_buffer_r_end - write_ptr;

                // wait for search thread consumed previous batch
                while (base36_buffer_w == local_base36_buffer_w || base36_buffer_w == 0) {
                        SW_MEM_BARRIER;
                        if (!run) { break; }
                        SLEEP;
                        sleep_count++;
                }
                local_base36_buffer_w = (char*) base36_buffer_w;
                write_ptr = local_base36_buffer_w;
                TRACE("36: consumed %p\n", local_base36_buffer_w);

                // wait for prime buffer ready
                while(prime_buffer_r == local_prime_buffer_r) {
                        SW_MEM_BARRIER;
                        if (!run) { break; }
                        SLEEP;
                        sleep_count++;
                }
                TRACE("36: ready\n");
        }

        printf("36: published %lu bytes, %lu sleep calls\n", byte_count, sleep_count);
        return 0;
}

void switch_base36_buffer()
{
        if (base36_buffer_w == base36_buffer[0]) {
                base36_buffer_w = base36_buffer[1];
        } else {
                base36_buffer_w = base36_buffer[0];
        }
        SW_MEM_BARRIER;
        HW_MEM_BARRIER;
}

const char* palindrome = 0;
int success = 0;
size_t total_offset = 0;

void* search_routine(void* arg)
{
        size_t sleep_count = 0;

        TRACE("search: started\n");
        // wait for base36 buffer ready first time
        while (base36_buffer_r == 0) {
                SW_MEM_BARRIER;
                //TRACE("search: base36_buffer_r=%p\n", base36_buffer_r);
                if (!run) { break; }
        }
        TRACE("search: ready 1\n");

        while(run) {
                // save base36 read ptr
                const char* local_base36_buffer_r = (const char*) base36_buffer_r;
                const char* local_base36_buffer_r_end = (const char*) base36_buffer_r_end;
                // let base36 thread proceed
                switch_base36_buffer();
                TRACE("search: switched %p\n", local_base36_buffer_r);

                // do work
                ssize_t offset = search_15_palindrome(local_base36_buffer_r, local_base36_buffer_r_end);
                if (offset >= 0) {
                        //printf("SUCCESS\n");
                        run = 0;
                        success = 1;
                        total_offset += offset;
                        palindrome = local_base36_buffer_r + offset;
                        break;
                }
                total_offset += local_base36_buffer_r_end - local_base36_buffer_r - 14;
                TRACE("search: done %lu\n", total_offset);

                if (total_offset > 1000*1000*1000) {
                        printf("STOP\n");
                        run = 0;
                        break;
                }

                // wait for base36 buffer ready
                while(base36_buffer_r == local_base36_buffer_r) {
                        SW_MEM_BARRIER;
                        if (!run) { break; }
                        SLEEP;
                        sleep_count++;
                }
                TRACE("search: ready\n");
        }

        printf("search: %lu sleep calls\n", sleep_count);
        return 0;
}


pthread_t gen_thread;
pthread_t base36_thread;


int main(int argc, char **argv) {
        clock_t begin, end;

        primesieve_init(&it);

        begin = clock();

        pthread_create(&gen_thread, NULL, &prime_gen_routine, NULL);
        pthread_create(&base36_thread, NULL, &base36_routine, NULL);

        search_routine(0);

        pthread_join(gen_thread, NULL);
        pthread_join(base36_thread, NULL);

        end = clock();

        primesieve_free_iterator(&it);

        if (success) {
                printf("\n");
                printf("Palindrome found ");
                for (size_t i = 0; i < 15; i++) {
                        printf("%c", palindrome[i]);
                }
                printf( " at byte %lu\n", total_offset);
        } else {
                printf("Failed\n");
        }

        printf("Time spend: %lf sec\n", (double) (end - begin) / CLOCKS_PER_SEC);
}
