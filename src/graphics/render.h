#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "world/map_priv.h"
#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>

// Load prerendered part of the map into framebuffer based on camera position
void load_prerendered(uint32_t *framebuffer, Map *map, Camera *camera);
void render_batch(uint32_t *framebuffer, RenderBatch *batch, Camera *camera);

#endif
