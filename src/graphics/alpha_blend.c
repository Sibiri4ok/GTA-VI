#include "alpha_blend.h"

// Accepts ARGB src and dst colors, returns ARGB blended color
uint32_t alpha_blend(uint32_t src, uint32_t dst) {
  uint8_t src_a = (src >> 24) & 0xFF;
  if (src_a == 0) return dst;

  if (src_a == 255) {
    return src;
  } else {
    uint8_t src_r = (src >> 16) & 0xFF;
    uint8_t src_g = (src >> 8) & 0xFF;
    uint8_t src_b = src & 0xFF;

    uint8_t dst_r = (dst >> 16) & 0xFF;
    uint8_t dst_g = (dst >> 8) & 0xFF;
    uint8_t dst_b = dst & 0xFF;

    uint8_t out_r = (src_r * src_a + dst_r * (255 - src_a)) / 255;
    uint8_t out_g = (src_g * src_a + dst_g * (255 - src_a)) / 255;
    uint8_t out_b = (src_b * src_a + dst_b * (255 - src_a)) / 255;

    return 0xFF000000 | (out_r << 16) | (out_g << 8) | out_b;
  }
}
