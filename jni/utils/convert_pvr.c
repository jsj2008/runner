#include <stdio.h>
#include <image.h>
#include <stream.h>

#define TAG_PVR               0x21525650

#define FLAG_TYPE_MASK        0xFF
#define FLAG_TYPE_PVRTC2      24
#define FLAG_TYPE_PVRTC4      25

#define max(a, b) (((a) > (b)) ? (a) : (b))

struct pvr_file_header_t
{
   unsigned long header_length;
   unsigned long height;
   unsigned long width;
   unsigned long nmipmaps;
   unsigned long flags;
   unsigned long data_length;
   unsigned long bpp;
   unsigned long bitmask_red;
   unsigned long bitmask_green;
   unsigned long bitmask_blue;
   unsigned long bitmask_alpha;
   unsigned long tag;
   unsigned long nsurfaces;
};

int image_load_from_pvr(image_t** pimage, const char* fname)
{
   LOGI("loading PVR image from %s", fname);

   stream_t* f = NULL;
   if (stream_open_reader(&f, fname) != 0)
   {
      return -1;
   }

   struct pvr_file_header_t header = {0};
   stream_read(f, &header, sizeof(header));

   if (header.tag != TAG_PVR)
   {
      LOGE("file is not a valid PVR");
      stream_close(f);
      return -1;
   }

   unsigned long type = header.flags & FLAG_TYPE_MASK;
   unsigned long format = IMAGE_RAW;
   unsigned long has_alpha = header.bitmask_alpha > 0;
   unsigned long block_width = 0;
   unsigned long block_height = 0;
   unsigned long block_bpp = 0;

   if (type == FLAG_TYPE_PVRTC2)
   {
      block_width = 8;
      block_height = 4;
      block_bpp = 2;

      if (has_alpha)
      {
         format = IMAGE_COMPRESSED_RGBA_PVRTC2;
      }
      else
      {
         format = IMAGE_COMPRESSED_RGB_PVRTC2;
      }
   }
   else if (type == FLAG_TYPE_PVRTC4)
   {
      block_width = 4;
      block_height = 4;
      block_bpp = 4;

      if (has_alpha)
      {
         format = IMAGE_COMPRESSED_RGBA_PVRTC4;
      }
      else
      {
         format = IMAGE_COMPRESSED_RGB_PVRTC4;
      }
   }
   else
   {
      LOGE("unsupported format: 0x%02lX", type);
      stream_close(f);
      return -1;
   }

   LOGI("header_len=%ld data_len=%ld width=%ld height=%ld bpp=%ld type=%ld format=%ld", header.header_length, header.data_length, header.width, header.height, header.bpp, type, format);

   unsigned long l = 0;
   unsigned long nmipmaps = header.nmipmaps + 1;
   unsigned long mipmaps_data_size = 0;
   unsigned long width = header.width;
   unsigned long height = header.height;

   for (l = 0; l < nmipmaps; ++l)
   {
      unsigned long block_size = block_width * block_height * block_bpp / 8;
      mipmaps_data_size += max(2, width / block_width) * max(2, height / block_height) * block_size;
      width /= 2;
      height /= 2;
   }

   LOGI("has_alpha=%ld nmipmaps=%ld mipmaps_data_size=%ld", has_alpha, nmipmaps, mipmaps_data_size);

   image_t* image = (image_t*)malloc(sizeof(image_t) + nmipmaps * sizeof(mipmap_t) + mipmaps_data_size);
   image->format = format;
   image->bpp = header.bpp;
   image->nmipmaps = nmipmaps;
   image->mipmaps = (mipmap_t*)((long)image + sizeof(image_t));

   unsigned long data_offset = (unsigned long)image->mipmaps + image->nmipmaps * sizeof(mipmap_t);

   width = header.width;
   height = header.height;
   mipmap_t* mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      unsigned long block_size = block_width * block_height * block_bpp / 8;
      mipmap->size = max(2, width / block_width) * max(2, height / block_height) * block_size;
      mipmap->width = width;
      mipmap->height = height;
      mipmap->data = (void*)data_offset;

      LOGI("mipmap #%ld: %ldx%ld %ld bytes", l, mipmap->width, mipmap->height, mipmap->size);

      stream_read(f, mipmap->data, mipmap->size);

      data_offset += mipmap->size;

      width /= 2;
      height /= 2;
   }

   (*pimage) = image;

   stream_close(f);

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

