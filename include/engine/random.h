#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

// Generates a pseudo-random float in range [0, 1] based on given x and y coordinates.
float rand01(int x, int y);
// Generates a random 32-bit unsigned integer.
uint32_t rand_big();

#endif
