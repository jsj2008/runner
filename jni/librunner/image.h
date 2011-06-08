#pragma once

typedef struct image_t
{
   int width;
   int height;
   int bpp;
   void* data;
} image_t;

int image_load_from_png(image_t** image, const char* fname);
void image_free(image_t* image);

