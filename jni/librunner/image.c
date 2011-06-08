#include "image.h"
#include "stream.h"
#include <png.h>

typedef struct png_read_data_t
{
   char* data;
   int offset;
} png_read_data_t;

static void png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_read_data_t* p = (png_read_data_t*)png_get_io_ptr(png_ptr);

   //LOGI("png_read: %d total: %d", length, p->offset);

   memcpy(data, p->data + p->offset, length);
   p->offset += length;
}

int image_load_from_png(image_t** pimage, const char* fname)
{
   long size = 0;
   char* data = (char*)stream_read_file(fname, &size);
   if (data == NULL)
      return -1;

   if (png_sig_cmp((png_bytep)data, 0, 8) != 0)
   {
      LOGE("File %s is not PNG", fname);
      free(data);
      return -1;
   }

   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL)
   {
      LOGE("Could not initialize png read struct");
      return -1;
   }

   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      LOGE("Could not initialize png info struct");
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return -1;
   }

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      LOGE("An error occured while reading the PNG file");
      return -1;
   }

   png_read_data_t p;
   p.data = data;
   p.offset = 0;

   png_set_read_fn(png_ptr, (voidp)&p, png_read);
   png_read_info(png_ptr, info_ptr);

   int width = png_get_image_width(png_ptr, info_ptr);
   int height = png_get_image_height(png_ptr, info_ptr);
   int color_type = png_get_color_type(png_ptr, info_ptr);
   int channels = png_get_channels(png_ptr, info_ptr);
   int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

   LOGI("Reading image [%dx%d:%d] color type %d channels %d", width, height, bit_depth, color_type, channels);

   switch (color_type)
   {
   case PNG_COLOR_TYPE_PALETTE:
      png_set_palette_to_rgb(png_ptr);
      channels = 3;
      break;

   case PNG_COLOR_TYPE_GRAY:
      png_set_gray_to_rgb(png_ptr);
      channels = 3;
      break;
   }

   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
   {
      png_set_tRNS_to_alpha(png_ptr);
      ++channels;
   }

   if (bit_depth == 16)
   {
      png_set_strip_16(png_ptr);
   }

   png_read_update_info(png_ptr, info_ptr);
   const unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
   unsigned char* img = (unsigned char*)malloc(rowbytes * height);

   png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
   int i = 0;
   for (i = 0; i < height; ++i)
   {
      row_pointers[height - i - 1] = (png_bytep)img + i * rowbytes;
   }

   png_read_image(png_ptr, row_pointers);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   free(row_pointers);
   free(data);

   image_t* image = (image_t*)malloc(sizeof(image_t));
   image->data = img;
   image->width = width;
   image->height = height;
   image->bpp = channels * 8;

   (*pimage) = image;

   return 0;
}

void image_free(image_t* image)
{
   free(image->data);
   free(image);
}

