#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <sys/types.h>

#define READ_ERR (-1)
#define READ_EOF (-2)

typedef float (*cos_derivative_fn)(float, float);
typedef int (*gcd_fn)(int, int);

typedef struct {
  int fd;
  char buf[4096];
  size_t len;
  size_t pos;
} line_reader_t;

ssize_t read_line(line_reader_t *r, char *out, size_t out_size);
void line_reader_init(line_reader_t *r, int fd);
void write_str(int fd, const char *s);
void die_errno(const char *msg);
void handle_command(const char *line, cos_derivative_fn cd, gcd_fn g);

#endif