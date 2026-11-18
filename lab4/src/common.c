#include "../include/common.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ssize_t read_line(line_reader_t *r, char *out, size_t out_size) {
  for (;;) {
    char *newline = memchr(r->buf + r->pos, '\n', r->len - r->pos);

    if (newline != NULL) {
      size_t line_len = newline - (r->buf + r->pos);
      if (out_size < line_len + 1) {
        errno = EOVERFLOW;
        return READ_ERR;
      }
      memcpy(out, r->buf + r->pos, line_len);
      out[line_len] = '\0';
      r->pos += line_len + 1;
      return line_len;
    } else {
      size_t remaining = r->len - r->pos;
      memmove(r->buf, r->buf + r->pos, remaining);
      r->pos = 0;
      r->len = remaining;

      if (r->len == sizeof(r->buf)) {
        errno = EOVERFLOW;
        return READ_ERR;
      }
      ssize_t n = read(r->fd, r->buf + r->len, sizeof(r->buf) - r->len);

      if (n < 0) {
        if (errno == EINTR) {
          continue;
        }
        return READ_ERR;
      }
      if (n == 0) {
        if (r->len == 0) {
          return READ_EOF;
        }
        if (out_size < r->len + 1) {
          errno = EOVERFLOW;
          return READ_ERR;
        }
        memcpy(out, r->buf, r->len);
        out[r->len] = '\0';
        size_t res = r->len;
        r->len = 0;
        return res;
      }
      r->len += n;
    }
  }
}

void line_reader_init(line_reader_t *r, int fd) {
  r->fd = fd;
  r->len = 0;
  r->pos = 0;
}

void write_str(int fd, const char *s) {
  write(fd, s, strlen(s));
}

void die_errno(const char *msg) {
  int e = errno;
  write_str(STDERR_FILENO, msg);
  write_str(STDERR_FILENO, ": ");
  write_str(STDERR_FILENO, strerror(e));
  write_str(STDERR_FILENO, "\n");
  _exit(1);
}

static int parse_float(const char **p, float *out) {
  char *end;
  *out = strtof(*p, &end);
  if (end == *p) return -1;
  *p = end;
  return 0;
}

static int parse_int(const char **p, int *out) {
  char *end;
  long v = strtol(*p, &end, 10);
  if (end == *p || v < 1 || v > INT_MAX) return -1;
  *out = (int)v;
  *p = end;
  return 0;
}

void handle_command(const char *line, cos_derivative_fn cd, gcd_fn g) {
  char out[64];
  char *end;
  long cmd = strtol(line, &end, 10);
  const char *p = end;

  if (cmd == 1) {
    float a, dx;
    if (parse_float(&p, &a) || parse_float(&p, &dx) || dx == 0) {
      write_str(STDERR_FILENO, "Usage: 1 a dx (dx != 0)\n");
      return;
    }
    snprintf(out, sizeof out, "%.6f\n", cd(a, dx));
    write_str(STDOUT_FILENO, out);
  } else if (cmd == 2) {
    int a, b;
    if (parse_int(&p, &a) || parse_int(&p, &b)) {
      write_str(STDERR_FILENO, "Usage: 2 a b (integers in 1..INT_MAX)\n");
      return;
    }
    snprintf(out, sizeof out, "%d\n", g(a, b));
    write_str(STDOUT_FILENO, out);
  } else {
    write_str(STDERR_FILENO, "Invalid command\n");
  }
}