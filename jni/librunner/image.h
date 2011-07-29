#pragma once

typedef struct mipmap_t
{
   unsigned long size;
   unsigned long width;
   unsigned long height;
   void* data;
} mipmap_t;

typedef struct image_t
{
   enum data_format_t
   {
      IMAGE_RAW = 0,
      IMAGE_COMPRESSED_RGB_ETC1,
      IMAGE_COMPRESSED_RGB_DXT1,
      IMAGE_COMPRESSED_RGBA_DXT1,
      IMAGE_COMPRESSED_RGBA_DXT3,
      IMAGE_COMPRESSED_RGBA_DXT5,
   } format;

   long bpp;
   unsigned long nmipmaps;
   mipmap_t* mipmaps;
} image_t;

int image_load_from_png(image_t** pimage, const char* fname);
int image_load(image_t** pimage, const char* fname);
int image_save(image_t* image, const char* fname);
void image_free(image_t* image);
