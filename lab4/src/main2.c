#include "../include/common.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

static const char *paths[] = {"./lib1.dylib", "./lib2.dylib"};

static void *load(const char *path, cos_derivative_fn *cd, gcd_fn *g) {
  void *h = dlopen(path, RTLD_NOW);
  if (h == NULL) return NULL;
  *cd = (cos_derivative_fn)dlsym(h, "cos_derivative");
  *g = (gcd_fn)dlsym(h, "gcd");
  if (*cd == NULL || *g == NULL) {
    dlclose(h);
    return NULL;
  }
  return h;
}

int main(void) {
  int cur = 0;
  cos_derivative_fn cd;
  gcd_fn g;
  void *h = load(paths[cur], &cd, &g);
  if (h == NULL) {
    write_str(STDERR_FILENO, "Cannot load library\n");
    return 1;
  }

  line_reader_t reader;
  line_reader_init(&reader, STDIN_FILENO);
  char line[2048];

  for (;;) {
    write_str(STDOUT_FILENO, "Enter command:\n");
    ssize_t n = read_line(&reader, line, sizeof line);
    if (n == READ_EOF) break;
    if (n == READ_ERR) die_errno("main2: read_line");
    if (n == 0) continue;

    char *end;
    long cmd = strtol(line, &end, 10);
    if (end != line && cmd == 0) {
      cos_derivative_fn ncd;
      gcd_fn ng;
      void *nh = load(paths[cur ^ 1], &ncd, &ng);
      if (nh == NULL) {
        write_str(STDERR_FILENO, "Cannot switch library\n");
        continue;
      }
      dlclose(h);
      h = nh; cd = ncd; g = ng; cur ^= 1;
      write_str(STDOUT_FILENO, cur ? "Switched to lib2\n" : "Switched to lib1\n");
    } else {
      handle_command(line, cd, g);
    }
  }
  dlclose(h);
  return 0;
}