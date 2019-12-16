#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "baseconversion.h"

#define BUF_SIZE 32

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
