#include "common.h"
#include "shared.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

int main(void) {
    line_reader_t r;
    line_reader_init(&r, STDIN_FILENO);
    char filename[256];
    ssize_t res = read_line(&r, filename, sizeof(filename));

    if (res < 0) {
        die_errno("parent: ошибка чтения строки");
    }
    if (res == 0) {
        const char msg[] = "parent: не введено имя файла\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
        _exit(1);
    }

    int fd_file = open(filename, O_RDONLY);
    if (fd_file < 0) {
        die_errno("parent: open");
    }

    pid_t me = getpid();
    char name_shm[64], name_full[64], name_empty[64];

    if (make_name(name_shm, sizeof(name_shm), me, "shm") < 0) {
        die_msg("parent: имя shm слишком длинное");
    }

    if (make_name(name_full, sizeof(name_full), me, "full") < 0) {
        die_msg("parent: имя full слишком длинное");
    }

    if (make_name(name_empty, sizeof(name_empty), me, "empty") < 0) {
        die_msg("parent: имя empty слишком длинное");
    }

    int fd_shm = shm_open(name_shm, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd_shm < 0) {
        die_errno("parent: shm_open");
    }

    if (ftruncate(fd_shm, sizeof(shared_t)) < 0) {
        die_errno("parent: ftruncate");
    }

    shared_t *slot = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (slot == MAP_FAILED) {
        die_errno("parent: mmap");
    }

    close(fd_shm);
    slot->state = 0;
    slot->value = 0;

    sem_t *sem_empty = sem_open(name_empty, O_CREAT | O_EXCL, 0600, 1);
    if (sem_empty == SEM_FAILED) {
        die_errno("parent: sem_open - name_empty");
    }
    sem_t *sem_full = sem_open(name_full, O_CREAT | O_EXCL, 0600, 0);
    if (sem_full == SEM_FAILED) {
        die_errno("parent: sem_open - name_full");
    }

    pid_t pid = fork();
    if (pid < 0) {
        die_errno("parent: fork");
    }
    if (pid == 0) {
        if (dup2(fd_file, STDIN_FILENO) < 0) {
            die_errno("child: dup2 stdin");
        }
        close(fd_file);
        char *argv[] = {"./child", name_shm, name_full, name_empty, NULL};
        execv("./child", argv);
        die_errno("child: execv");
    }
    close(fd_file);
    int status;
    for (;;) {
        if (sem_wait(sem_full) < 0) {
            if (errno == EINTR) {
                continue;
            }
            die_errno("parent: sem_wait");
        }
        int state = slot->state;
        long value = slot->value;

        if (sem_post(sem_empty) < 0) {
            die_errno("parent: sem_post");
        }

        if (state == SLOT_END) {
            break;
        }
        if (state == SLOT_DATA) {
            write_long_line(STDOUT_FILENO, value);
        } else if (state == SLOT_ERROR) {
            write_error_line(STDOUT_FILENO, value);
        }
    }

    waitpid(pid, &status, 0);

    munmap(slot, sizeof(shared_t));

    sem_close(sem_empty);
    sem_close(sem_full);

    sem_unlink(name_empty);
    sem_unlink(name_full);
    shm_unlink(name_shm);

    return 0;
}