#ifndef DISPLAY_H
#define DISPLAY_H

#include <engine/input.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Display Display;

Display *display_create(int width, int height, float scale, const char *title);
void display_free(Display *d);

// Poll events and send them to input entity
bool display_poll_events(Input *input);
// Display the given frame buffer
void display_present(Display *d, const uint32_t *pixels);

// Time between last two frames in milliseconds
uint64_t display_get_delta_time(Display *d);
// Time of last displayed frame in milliseconds
uint64_t display_get_last_frame_time(Display *d);

uint64_t display_get_ticks();

#endif
