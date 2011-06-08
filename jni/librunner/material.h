#pragma once

typedef struct material_t
{
   char name[64];
   char shader[64];
   char texture[64];
} material_t;

void material_show(const material_t* material);
void material_bind(const material_t* material, int sampler_id);
void material_unbind(const material_t* material);

