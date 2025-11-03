#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef struct {
  int action_count;
  bool w, a, s, d;
  bool quit;
} Input;

#endif
