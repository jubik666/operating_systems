#include "../include/common.h"
#include "../include/lib.h"
#include <unistd.h>

int main(void) {
  line_reader_t reader;
  line_reader_init(&reader, STDIN_FILENO);
  char line[2048];

  for (;;) {
    write_str(STDOUT_FILENO, "Enter command:\n");
    ssize_t n = read_line(&reader, line, sizeof line);
    if (n == READ_EOF) break;
    if (n == READ_ERR) die_errno("main1: read_line");
    if (n > 0) handle_command(line, cos_derivative, gcd);
  }
  return 0;
}