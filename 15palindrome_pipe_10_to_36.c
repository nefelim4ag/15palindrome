#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "baseconversion.h"
#include "stdin.h"

#define BUF_SIZE 32

int main(int argc, char **argv) {
        static char output_buf[BUF_SIZE];

        while (1) {
                char *ptr = get_stdin_line();
                if (ptr == NULL)
                        break;
                base_10_to_36(output_buf, ptr);
                printf("%s\n", output_buf);
        }
        return 0;
}
