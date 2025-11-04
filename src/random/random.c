#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

uint32_t hash_u32(int x, int y) {
  uint32_t h = (uint32_t)(x * 374761393u + y * 668265263u);
  h = (h ^ (h >> 13)) * 1274126177u;
  return h ^ (h >> 16);
}

float rand01(int x, int y) {
  return (hash_u32(x, y) & 0xFFFF) / 65535.0f;
}

static bool random_initialized = false;

// Generates a random 32-bit unsigned integer
uint32_t rand_big() {
  if (!random_initialized) {
    srand((unsigned int)time(NULL));
    random_initialized = true;
  }
  uint32_t result = 0;
  result = (rand() & 0x7FFF);
  result |= (rand() & 0x7FFF) << 15;
  result |= (rand() & 0x0003) << 30;
  return result;
}
