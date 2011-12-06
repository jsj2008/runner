#include "tex2d.h"
#include "image.h"
#include "common.h"
#include "gl_defs.h"

void load_compressed_image(const struct image_t* image, int internalFormat)
{
   unsigned long level = 0;
   const mipmap_t* mipmap = &image->mipmaps[0];
   for (level = 0; level < image->nmipmaps; ++level, ++mipmap)
   {
      //LOGI("glCompressedTexImage2D %ld [%ldx%ld] %ld bytes", level, mipmap->width, mipmap->height, mipmap->size);
      glCompressedTexImage2D(GL_TEXTURE_2D, level, internalFormat, mipmap->width, mipmap->height, 0, mipmap->size, mipmap->data);
      checkGLError("glCompressedTexImage2D");
   }
}

int tex2d_create(tex2d_t** ptexture, const struct image_t* image, int min_filter, int mag_filter, int wrap_s, int wrap_t)
{
   GLuint id = 0;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   if (min_filter == GL_LINEAR_MIPMAP_LINEAR ||
         min_filter == GL_NEAREST_MIPMAP_LINEAR ||
         min_filter == GL_LINEAR_MIPMAP_NEAREST ||
         min_filter == GL_NEAREST_MIPMAP_NEAREST)
   {
      if (image->format != IMAGE_RAW && image->nmipmaps == 1)
      {
         min_filter = GL_LINEAR;
      }
   }

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
   //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_HINT, GL_NICEST);

   GLint format = (image->bpp == 32) ? GL_RGBA : GL_RGB;

   long level = 0;
   const mipmap_t* mipmap = NULL;

   switch (image->format)
   {
   case IMAGE_RAW:
      mipmap = &image->mipmaps[0];
      for (level = 0; level < image->nmipmaps; ++level, ++mipmap)
      {
         glTexImage2D(GL_TEXTURE_2D, level, format, mipmap->width, mipmap->height, 0, format, GL_UNSIGNED_BYTE, mipmap->data);
         checkGLError("glTexImage2D");
      }

      if (image->nmipmaps == 1)
      {
         glGenerateMipmap(GL_TEXTURE_2D);
         checkGLError("glGenerateMipmap");
      }
      break;

   case IMAGE_COMPRESSED_RGB_ETC1:
      load_compressed_image(image, GL_ETC1_RGB8_OES);
      break;

   case IMAGE_COMPRESSED_RGB_DXT1:
      load_compressed_image(image, GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
      break;

   case IMAGE_COMPRESSED_RGBA_DXT1:
      load_compressed_image(image, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
      break;

   case IMAGE_COMPRESSED_RGBA_DXT3:
      load_compressed_image(image, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
      break;

   case IMAGE_COMPRESSED_RGBA_DXT5:
      load_compressed_image(image, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
      break;

   case IMAGE_COMPRESSED_RGB_ATC:
      load_compressed_image(image, GL_ATC_RGB_AMD);
      break;

   case IMAGE_COMPRESSED_RGBA_ATCE:
      load_compressed_image(image, GL_ATC_RGBA_EXPLICIT_ALPHA_AMD);
      break;

   case IMAGE_COMPRESSED_RGBA_ATCI:
      load_compressed_image(image, GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD);
      break;

   case IMAGE_COMPRESSED_RGB_PVRTC2:
      load_compressed_image(image, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
      break;

   case IMAGE_COMPRESSED_RGB_PVRTC4:
      load_compressed_image(image, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
      break;

   case IMAGE_COMPRESSED_RGBA_PVRTC2:
      load_compressed_image(image, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG);
      break;

   case IMAGE_COMPRESSED_RGBA_PVRTC4:
      load_compressed_image(image, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
      break;

   default:
      LOGE("Unsupported image format %d", image->format);
      return -1;
   }

   LOGI("Loaded texture id = %d", id);

   (*ptexture) = (tex2d_t*)id;

   return 0;
}

void tex2d_free(tex2d_t* texture)
{
   GLuint id = (GLuint)texture;
   if (id > 0)
   {
      glDeleteTextures(1, &id);
   }
}

int tex2d_bind(tex2d_t* texture, int sampler)
{
   GLuint id = (GLuint)texture;

   glActiveTexture(GL_TEXTURE0 + sampler);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   return 0;
}

