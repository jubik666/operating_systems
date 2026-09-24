#include <unistd.h>
#include <stdlib.h>
#include "common.h"

int is_prime(long n);
long parse_long(const char *str);
int write_long_line(int fd, long n);
void write_error_line(int fd, long n);

int main(void) {
    line_reader_t r;
    line_reader_init(&r, STDIN_FILENO);

    char line[256];

    for (;;) {
        ssize_t res = read_line(&r, line, sizeof(line));

        if (res < 0) {
            die_errno("child: ошибка чтения строки");
        }

        if (res == 0) {
            return 0;
        }
        long number = parse_long(line);

        if (number == 0 || number == 1) {
            write_error_line(STDOUT_FILENO, number);
            continue;
        }

        if (number < 0 || is_prime(number)) {
            _exit(0);
        }
        write_long_line(STDOUT_FILENO, number);
    }
}

int is_prime(long n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

long parse_long(const char *str) {
    return strtol(str, NULL, 10);
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