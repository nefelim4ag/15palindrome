#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
