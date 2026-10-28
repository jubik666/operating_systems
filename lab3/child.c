#include "common.h"
#include "shared.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>

int is_prime(long n);
long parse_long(const char *str);
void send_slot(shared_t *slot, sem_t *sem_empty, sem_t *sem_full, int state, long value);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        die_msg("child: not arguments");
    }

    int fd_shm = shm_open(argv[1], O_RDWR, 0);
    if (fd_shm < 0) {
        die_errno("child: shm_open");
    }

    shared_t *slot = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (slot == MAP_FAILED) {
        die_errno("child: mmap");
    }
    close(fd_shm);

    sem_t *sem_full = sem_open(argv[2], 0);
    if (sem_full == SEM_FAILED) {
        die_errno("child: sem_open - sem_full");
    }
    sem_t *sem_empty = sem_open(argv[3], 0);
    if (sem_empty == SEM_FAILED) {
        die_errno("child: sem_open - sem_empty");
    }

    line_reader_t r;
    line_reader_init(&r, STDIN_FILENO);

    char line[256];

    for (;;) {
        ssize_t res = read_line(&r, line, sizeof(line));

        if (res < 0) {
            die_errno("child: ошибка чтения строки");
        }

        if (res == 0) {
            break;
        }
        long number = parse_long(line);

        if (number == 0 || number == 1) {
            send_slot(slot, sem_empty, sem_full, SLOT_ERROR, number);
            continue;
        }

        if (number < 0 || is_prime(number)) {
            break;
        }
        send_slot(slot, sem_empty, sem_full, SLOT_DATA, number);
    }
    send_slot(slot, sem_empty, sem_full, SLOT_END, 0);
    munmap(slot, sizeof(shared_t));
    sem_close(sem_empty);
    sem_close(sem_full);
    return 0;
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

void send_slot(shared_t *slot, sem_t *sem_empty, sem_t *sem_full, int state, long value) {
    while (sem_wait(sem_empty) < 0) {
        if (errno == EINTR) {
            continue;
        }
        die_errno("child: sem_wait");
    }
    slot->state = state;
    slot->value = value;
    
    if (sem_post(sem_full) < 0) {
        die_errno("child: sem_post");
    }
}