#pragma once

#include "physworld.h"

struct cam_t;

typedef struct entity_t
{
   char model[64];
   int anim;
   int frame;

   rigidbody_t* phys;
} entity_t;

void entity_update(entity_t* e, float dt);
void entity_render(const entity_t* e, const struct cam_t* camera);

