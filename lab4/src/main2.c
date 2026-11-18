#include "../include/common.h"
#include <dlfcn.h>
#include <unistd.h>

static cos_derivative_fn cos_ptr;
static gcd_fn gcd_ptr;

static void *load(const char *path) {
  void *h = dlopen(path, RTLD_NOW);
  if (h == NULL) return NULL;

  cos_derivative_fn c = (cos_derivative_fn)dlsym(h, "cos_derivative");
  gcd_fn g = (gcd_fn)dlsym(h, "gcd");
  if (c == NULL || g == NULL) {
    dlclose(h);
    return NULL;
  }
  cos_ptr = c;
  gcd_ptr = g;
  return h;
}

int main(void) {
  int current = 1;
  void *handle = load("./lib1.dylib");
  if (handle == NULL) {
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

    if (line[0] == '0') {
      const char *path = (current == 1) ? "./lib2.dylib" : "./lib1.dylib";
      void *new_handle = load(path);
      if (new_handle == NULL) {
        write_str(STDERR_FILENO, "Cannot switch library\n");
        continue;
      }
      dlclose(handle);
      handle = new_handle;
      current = (current == 1) ? 2 : 1;
      write_str(STDOUT_FILENO, "Library switched\n");
    } else {
      handle_command(line, cos_ptr, gcd_ptr);
    }
  }
  dlclose(handle);
  return 0;
}