#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>
#include <stddef.h>

typedef enum {
  SLOT_DATA = 1,
  SLOT_ERROR = 2,
  SLOT_END = 3
} slot_state_t;

typedef struct {
  int state;
  long value;
} shared_t;

int make_name(char *buf, size_t size, pid_t pid, const char *suffix);

#endif