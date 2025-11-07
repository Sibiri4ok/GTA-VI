#include "alpha_blend.h"

// Accepts ARGB top and bot colors, returns ARGB blended color
inline uint32_t alpha_blend(uint32_t top, uint32_t bot) {
  uint32_t top_a = (top >> 24);
  if (top_a == 0) return bot;
  if (top_a == 255) return top;

  uint32_t inv_a = 255 - top_a;

  // red and blue channels together for optimization
  uint32_t rb = (((top & 0x00FF00FF) * top_a + (bot & 0x00FF00FF) * inv_a) >> 8) & 0x00FF00FF;
  uint32_t g = (((top & 0x0000FF00) * top_a + (bot & 0x0000FF00) * inv_a) >> 8) & 0x0000FF00;
  return 0xFF000000 | rb | g;
}
