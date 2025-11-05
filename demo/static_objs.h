#ifndef STATIC_OBJS_H
#define STATIC_OBJS_H

#include <engine/map.h>
#include <engine/types.h>

typedef struct StaticObjects {
  Sprite *sprites;
  GameObject *objects;
} StaticObjects;

StaticObjects *create_static_objs(Map *map, int count);
void free_static_objs(StaticObjects *st_objs);

#endif
