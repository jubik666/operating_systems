#include "common.h"
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