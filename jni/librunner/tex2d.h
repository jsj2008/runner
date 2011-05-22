#pragma once

typedef struct tex2d_t tex2d_t;

int tex2d_load_from_png(tex2d_t** t, const char* fname);
int tex2d_load_checkered_texture(tex2d_t** t);
void tex2d_free(tex2d_t* t);
void tex2d_set_params(tex2d_t* t, int min_filter, int mag_filter, int wrap_s, int wrap_t);
int tex2d_bind(tex2d_t* t, int sampler);
