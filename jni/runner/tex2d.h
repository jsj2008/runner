#pragma once

#include "common.h"

typedef struct tex2d_t
{
   int width;
   int height;
   int bpp;
   unsigned char* data;
} tex2d_t;

int tex2d_load_from_png(tex2d_t* t, const char* fname);
void tex2d_free(tex2d_t* t);

