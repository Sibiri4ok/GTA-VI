#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "world/map_priv.h"
#include <engine/types.h>
#include <stdbool.h>
#include <stdint.h>

int compare_objs_by_depth(const void *a, const void *b);
void render_objects(uint32_t *framebuffer, GameObject **objects, int count, Camera *camera);
void load_prerendered(uint32_t *framebuffer, Map *map, Camera *camera);

#endif
