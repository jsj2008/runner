#include <stdio.h>
#include <CPVRTextureHeader.h>

extern "C"
{
   #include <image.h>
   #include <stream.h>
}

using namespace pvrtexlib;

image_t::data_format_t pixelType2imageFormat(PixelType type, bool hasAlpha)
{
   switch (type)
   {
      case OGL_PVRTC2:
      case MGLPT_PVRTC2:
         return hasAlpha ? image_t::IMAGE_COMPRESSED_RGBA_PVRTC2 : image_t::IMAGE_COMPRESSED_RGB_PVRTC2;

      case OGL_PVRTC4:
      case MGLPT_PVRTC4:
         return hasAlpha ? image_t::IMAGE_COMPRESSED_RGBA_PVRTC4 : image_t::IMAGE_COMPRESSED_RGB_PVRTC4;

      case ETC_RGB_4BPP:
         return image_t::IMAGE_COMPRESSED_RGB_ETC1;

      default:
         return image_t::IMAGE_RAW;
   }
}

int image_load_from_pvr(image_t** pimage, const char* fname)
{
   LOGI("loading PVR image from %s", fname);

   long size = 0;
   void* data = stream_read_file(fname, &size);
   if (data == NULL)
   {
      return -1;
   }

   CPVRTextureHeader header ((const uint8*)data);
   unsigned long l = 0;
   unsigned long nmipmaps = header.getMipMapCount() + 1;
   unsigned long mipmaps_data_size = 0;
   image_t::data_format_t format = pixelType2imageFormat(header.getPixelType(), header.hasAlpha());

   LOGI("Has alpha: %d", header.hasAlpha());

   if (format == image_t::IMAGE_RAW)
   {
      LOGE("Unsupported pixel format: %d", header.getPixelType());
      return -1;
   }

   for (l = 0; l < nmipmaps; ++l)
   {
      mipmaps_data_size += header.getMIPSize(l);
   }

   image_t* image = (image_t*)malloc(sizeof(image_t) + nmipmaps * sizeof(mipmap_t) + mipmaps_data_size);
   image->format = format;
   image->bpp = header.getBitsPerPixel();
   image->nmipmaps = nmipmaps;
   image->mipmaps = (mipmap_t*)((long)image + sizeof(image_t));

   unsigned long data_offset = (unsigned long)image->mipmaps + image->nmipmaps * sizeof(mipmap_t);
   unsigned char* pvr_data = (unsigned char*)data + header.getFileHeaderSize();

   mipmap_t* mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      mipmap->size = header.getMIPSize(l);
      mipmap->width = header.getMipWidth(l);
      mipmap->height = header.getMipHeight(l);
      mipmap->data = (void*)data_offset;

      LOGI("mipmap #%ld: %ldx%ld %ld bytes", l, mipmap->width, mipmap->height, mipmap->size);

      memcpy(mipmap->data, pvr_data, mipmap->size);

      pvr_data += mipmap->size;
      data_offset += mipmap->size;
   }

   (*pimage) = image;

   free(data);

   return 0;
}

int main(int argc, const char** argv)
{
   if (argc < 2)
   {
      printf("usage: ./convert_pvr file.pvr\n");
      return 1;
   }

   int i = 0;
   for (i = 1; i < argc; ++i)
   {
      image_t* img = NULL;
      if (image_load_from_pvr(&img, argv[i]) == 0)
      {
         char fname[256] = {0};
         strcpy(fname, argv[i]);
         char* end = strrchr(fname, '.');
         if (end != NULL)
         {
            strcpy(end, ".texture");
         }

         image_save(img, fname);
         image_free(img);
      }
   }
   return 0;
}

