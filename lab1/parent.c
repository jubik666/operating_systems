#include "common.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

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

    int pipe1[2];
    if (pipe(pipe1) < 0) {
        die_errno("parent: pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        die_errno("parent: fork");
    }
    if (pid == 0) {
        if (dup2(fd_file, STDIN_FILENO) < 0) {
            die_errno("child: dup2 stdin");
        }
        if (dup2(pipe1[1], STDOUT_FILENO) < 0) {
            die_errno("child: dup2 stdout");
        }
        close(fd_file);
        close(pipe1[0]);
        close(pipe1[1]);
        char *argv[] = {"./child", NULL};
        execv("./child", argv);
        die_errno("child: execv");
    }
    close(fd_file);
    close(pipe1[1]);
    char buf[4096];
    ssize_t n;
    while ((n = read(pipe1[0], buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    if (n < 0) {
        die_errno("parent: read");
    }
    close(pipe1[0]);
    int status;
    waitpid(pid, &status, 0);
    return 0;
}