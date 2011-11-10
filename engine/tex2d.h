#pragma once

struct image_t;

typedef struct tex2d_t tex2d_t;

int tex2d_create(tex2d_t** ptexture, const struct image_t* image, int min_filter, int mag_filter, int wrap_s, int wrap_t);
void tex2d_free(tex2d_t* texture);
int tex2d_bind(tex2d_t* texture, int sampler);

