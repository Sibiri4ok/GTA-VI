#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Display Display;
typedef struct Input Input;

Display *display_create(int width, int height, const char *title);
void display_destroy(Display *d);

bool display_poll_events(Display *d, Input *input);
void display_present(Display *d, const uint32_t *pixels);

float display_get_fps(Display *d);

#endif
