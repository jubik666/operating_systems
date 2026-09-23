#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <sys/types.h>

typedef struct {
    int fd;
    char buf[4096];
    size_t len;
    size_t pos;
} line_reader_t;

ssize_t read_line(line_reader_t *r, char *out, size_t out_size);

void line_reader_init(line_reader_t *r, int fd);

void die_errno(const char *msg);

#endif