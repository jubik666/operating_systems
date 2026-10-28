#include "common.h"
#include "shared.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

ssize_t read_line(line_reader_t *r, char *out, size_t out_size) {
    for (;;) {
        char *newline = memchr(r->buf + r->pos, '\n', r->len - r->pos);

        if (newline != NULL) {
            size_t line_len = newline - (r->buf + r->pos);
            if (out_size < line_len + 1) {
                errno = EOVERFLOW;
                return -1;
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
                return -1;
            }
            ssize_t n = read(r->fd, r->buf + r->len, sizeof(r->buf) - r->len);
            if (n < 0) {
                return -1;
            }
            if (n == 0) {
                if (r->len == 0) {
                    return 0;
                } else {
                    if (out_size < r->len + 1) {
                        errno = EOVERFLOW;
                        return -1;
                    }
                    memcpy(out, r->buf, r->len);
                    out[r->len] = '\0';
                    size_t res = r->len;
                    r->len = 0;
                    return res;
                }
            }
            if (n > 0) {
                r->len += n;
            }
        }
    }
}

void line_reader_init(line_reader_t *r, int fd) {
    r->fd = fd;
    r->len = 0;
    r->pos = 0;
}

void die_errno(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, ": ", 2);
    const char *err_text = strerror(errno);
    write(STDERR_FILENO, err_text, strlen(err_text));
    write(STDERR_FILENO, "\n", 1);
    _exit(1);
}

int make_name(char *buf, size_t size, pid_t pid, const char *suffix) {
    int n = snprintf(buf, size, "/lab3_%d_%s", pid, suffix);
    if (n < 0 || (size_t)n >= size) {
        return -1;
    }
    return 0;
}

void die_msg(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
    _exit(1);
}

int write_long_line(int fd, long n) {
    char buf[32];
    int pos = sizeof(buf);
    --pos;
    buf[pos] = '\n';
    if (n == 0) buf[--pos] = '0';
    while (n != 0) {
        long a = n % 10;
        buf[--pos] = '0' + a;
        n /= 10;
    }
    write(fd, buf + pos, sizeof(buf) - pos);
    return 0;
}

void write_error_line(int fd, long n) {
    char num_buf[32];
    int pos = sizeof(num_buf);
    if (n == 0) {
        num_buf[--pos] = '0';
    } else {
        long tmp = n;
        while (tmp != 0) {
            num_buf[--pos] = '0' + (tmp % 10);
            tmp /= 10;
        }
    }
    const char prefix[] = "ERROR: ";
    const char suffix[] = " is neither prime nor composite\n";
    write(fd, prefix, sizeof(prefix) - 1);
    write(fd, num_buf + pos, sizeof(num_buf) - pos);
    write(fd, suffix, sizeof(suffix) - 1);
}