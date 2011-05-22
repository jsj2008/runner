#include "tex2d.h"
#include "stream.h"
#include "gl.h"
#include <png.h>

struct tex2d_t
{
   GLuint id;
   int width;
   int height;
   int bpp;
   unsigned char* data;

   int min_filter;
   int mag_filter;
   int wrap_s;
   int wrap_t;
};

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

int tex2d_load_from_png(tex2d_t** pt, const char* fname)
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

   tex2d_t* t = (tex2d_t*)malloc(sizeof(tex2d_t));
   t->data = img;
   t->width = width;
   t->height = height;
   t->bpp = channels * 8;
   t->id = -1;
   t->min_filter = GL_LINEAR_MIPMAP_LINEAR;
   t->mag_filter = GL_LINEAR;
   t->wrap_s = GL_REPEAT;
   t->wrap_t = GL_REPEAT;

   (*pt) = t;

   return 0;
}

int tex2d_load_checkered_texture(tex2d_t** pt)
{
   unsigned char pixels[] =
   {
      127, 127, 127,
      0, 0, 0,
      0, 0, 0,
      127, 127, 127,
   };

   tex2d_t* t = (tex2d_t*)malloc(sizeof(tex2d_t));
   t->data = pixels;
   t->width = 2;
   t->height = 2;
   t->bpp = 3 * 8;
   t->id = -1;
   t->min_filter = GL_NEAREST_MIPMAP_NEAREST;
   t->mag_filter = GL_NEAREST;
   t->wrap_s = GL_REPEAT;
   t->wrap_t = GL_REPEAT;

   (*pt) = t;
   return 0;
}

void tex2d_free(tex2d_t* t)
{
   if (t->id > 0)
   {
      glDeleteTextures(1, &t->id);
   }

   free(t->data);
   memset(t, 0, sizeof(tex2d_t));
}

void tex2d_set_params(tex2d_t* t, int min_filter, int mag_filter, int wrap_s, int wrap_t)
{
   t->min_filter = min_filter;
   t->mag_filter = mag_filter;
   t->wrap_s = wrap_s;
   t->wrap_t = wrap_t;
}

int tex2d_bind(tex2d_t* t, int sampler)
{
   if (t->id == -1)
   {
      GLuint id = 0;
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      checkGLError("glPixelStorei");

      glGenTextures(1, &id);
      checkGLError("glGenTextures");

      glBindTexture(GL_TEXTURE_2D, id);
      checkGLError("glBindTexture");

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, t->min_filter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, t->mag_filter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, t->wrap_s);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t->wrap_t);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->width, t->height, 0, GL_RGB, GL_UNSIGNED_BYTE, t->data);
      checkGLError("glTexImage2D");

      glGenerateMipmap(GL_TEXTURE_2D);
      checkGLError("glGenerateMipmap");

      LOGI("Loaded texture id = %d", id);
      t->id = id;
   }

   glActiveTexture(GL_TEXTURE0 + sampler);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, t->id);
   checkGLError("glBindTexture");

   return 0;
}
