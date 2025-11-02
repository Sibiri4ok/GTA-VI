#include "graphics/render.h"
#include <game/map.h>
#include <graphics/alpha_blend.h>
#include <graphics/camera.h>
#include <graphics/coordinates.h>
#include <stdlib.h>

void render_object(uint32_t *framebuffer, Camera *camera, GameObject *object) {
  if (!framebuffer || !object) return;
  Sprite *sprite = object->sprite;

  for (int sy = 0; sy < sprite->height; sy++) {
    for (int sx = 0; sx < sprite->width; sx++) {
      int src_x = object->flip_horizontal ? (sprite->width - 1 - sx) : sx;

      Vector2 object_screen_coord = camera_world_to_screen(camera, object->position);
      int screen_x = object_screen_coord.x + sx;
      int screen_y = object_screen_coord.y + sy;

      if (!camera_is_visible(camera, (Vector2){screen_x, screen_y})) { continue; }

      uint32_t src = sprite->pixels[sy * sprite->width + src_x];
      int fb_idx = screen_y * camera->size.x + screen_x;
      uint32_t dst = framebuffer[fb_idx];
      framebuffer[fb_idx] = alpha_blend(src, dst);
    }
  }
}

void render_frame_static(Map *map, uint32_t *framebuffer, Camera *camera) {
  if (!map || !framebuffer || !camera) return;

  // Offsets to center the map in pixel coordinates
  int offset_x = map->height * (ISO_TILE_WIDTH / 2);
  int offset_y = map->height * (ISO_TILE_HEIGHT / 2);

  // Calculate top-left corner of visible area in map coordinates (once!)
  Vector2 top_left_world = camera_screen_to_world(camera, (Vector2){0, 0});
  int map_start_x = (int)top_left_world.x + offset_x;
  int map_start_y = (int)top_left_world.y + offset_y;

  // Render visible part of the map
  for (int screen_y = 0; screen_y < camera->size.y; screen_y++) {
    for (int screen_x = 0; screen_x < camera->size.x; screen_x++) {
      int map_x = map_start_x + screen_x;
      int map_y = map_start_y + screen_y;

      // Skip pixels outside map
      if (map_x < 0 || map_x >= map->width_pix || map_y < 0 || map_y >= map->height_pix) {
        continue;
      }
      int idx = screen_y * (int)camera->size.x + screen_x;
      uint32_t pixel = map->pixels[map_y * map->width_pix + map_x];
      framebuffer[idx] = alpha_blend(pixel, framebuffer[idx]);
    }
  }
}
