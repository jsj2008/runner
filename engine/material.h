#pragma once

#include "mathlib.h"

typedef struct vec3f_t color_t;

typedef struct material_t
{
   char name[64];
   char shader[64];
   char texture[64];

   color_t diffuse;
   color_t specular;
   float shininess;
} material_t;

void material_show(const material_t* material);
void material_bind(const material_t* material, int sampler_id);
void material_unbind(const material_t* material);

