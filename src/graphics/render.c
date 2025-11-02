#include "graphics/render.h"
#include <game/map.h>
#include <graphics/alpha_blend.h>
#include <graphics/coordinates.h>
#include <stdlib.h>

void draw_sprite(uint32_t *framebuffer,
    int fb_width,
    int fb_height,
    Sprite *sprite,
    int x,
    int y,
    bool flip_horizontal) {
  if (!framebuffer || !sprite) return;

  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int src_x = flip_horizontal ? (sprite->width - 1 - sx) : sx;

      int screen_x = x + sx;
      int screen_y = y + sy;

      if (screen_x < 0 || screen_x >= fb_width || screen_y < 0 || screen_y >= fb_height) {
        continue;
      }

      uint32_t src = sprite->pixels[sy * sprite->width + src_x];
      int fb_idx = screen_y * fb_width + screen_x;
      uint32_t dst = framebuffer[fb_idx];
      framebuffer[fb_idx] = alpha_blend(src, dst);
    }
  }
}

void render_frame_static(Map *map,
    uint32_t *framebuffer,
    int frame_width,
    int frame_height,
    Vector2 camera_pos) {
  if (!map || !framebuffer) return;

  // Offsets to center the map in pixel coordinates
  int offset_x = map->height * (ISO_TILE_WIDTH / 2);
  int offset_y = map->height * (ISO_TILE_HEIGHT / 2);

  // Calculate top-left corner of visible area in map coordinates
  int map_start_x = (int)camera_pos.x + offset_x - frame_width / 2;
  int map_start_y = (int)camera_pos.y + offset_y - frame_height / 2;

  // Render visible part of the map
  for (int screen_y = 0; screen_y < frame_height; screen_y++) {
    for (int screen_x = 0; screen_x < frame_width; screen_x++) {
      int map_x = map_start_x + screen_x;
      int map_y = map_start_y + screen_y;

      // Skip pixels outside map
      if (map_x < 0 || map_x >= map->width_pix || map_y < 0 || map_y >= map->height_pix) {
        continue;
      }
      int idx = screen_y * frame_width + screen_x;
      uint32_t pixel = map->pixels[map_y * map->width_pix + map_x];
      framebuffer[idx] = alpha_blend(pixel, framebuffer[idx]);
    }
  }
}
