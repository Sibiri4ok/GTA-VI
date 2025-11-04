#ifndef DISPLAY_H
#define DISPLAY_H

#include <engine/input.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Display Display;

Display *display_create(int width, int height, float scale, const char *title);
void display_free(Display *d);

bool display_poll_events(Input *input);
void display_present(Display *d, const uint32_t *pixels);

float display_get_fps(Display *d);
uint64_t display_get_ticks(void);

#endif
