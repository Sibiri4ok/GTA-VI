#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

// Currently pressed buttons.
typedef struct {
  bool up, down, left, right;
  bool q, w, e, r, t, y, u, i, o, p;
  bool a, s, d, f, g, h, j, k, l;
  bool z, x, c, v, b, n, m;
  bool space, enter, lctrl, lshift;
  bool quit;
} Input;

#endif
