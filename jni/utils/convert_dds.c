#include <stdio.h>
#include <image.h>
#include <stream.h>

#define DDS_MAGIC                   0x20534444

#define DDSD_CAPS                   0x00000001
#define DDSD_HEIGHT                 0x00000002
#define DDSD_WIDTH                  0x00000004
#define DDSD_PITCH                  0x00000008
#define DDSD_PIXELFORMAT            0x00001000
#define DDSD_MIPMAPCOUNT            0x00020000
#define DDSD_LINEARSIZE             0x00080000
#define DDSD_DEPTH                  0x00800000

#define DDPF_ALPHAPIXELS            0x00000001
#define DDPF_FOURCC                 0x00000004
#define DDPF_INDEXED                0x00000020
#define DDPF_RGB                    0x00000040

#define D3DFMT_DXT1                 0x31545844
#define D3DFMT_DXT3                 0x33545844
#define D3DFMT_DXT5                 0x35545844

#define max(a, b) (((a) > (b)) ? (a) : (b))

struct dds_file_header_t
{
   unsigned int magic;
   unsigned int size;
   unsigned int flags;
   unsigned int height;
   unsigned int width;
   unsigned int pitch;
   unsigned int depth;
   unsigned int nmipmaps;
   unsigned int reserved1[11];

   struct
   {
      unsigned int size;
      unsigned int flags;
      unsigned int fourcc;
      unsigned int rgbbitcount;
      unsigned int rbitmask;
      unsigned int gbitmask;
      unsigned int bbitmask;
      unsigned int abitmask;
   } pixel_format;

   struct
   {
      unsigned int caps1;
      unsigned int caps2;
      unsigned int ddsx;
      unsigned int reserved;
   } caps;

   unsigned int reserved2;
};

int image_load_from_dds(image_t** pimage, const char* fname)
{
   LOGI("loading DDS image from %s", fname);

   stream_t* f = NULL;
   if (stream_open_reader(&f, fname) != 0)
   {
      return -1;
   }

   struct dds_file_header_t header;
   stream_read(f, &header, sizeof(header));

   if (  header.magic != DDS_MAGIC ||
         header.size != 124 ||
         (header.flags & DDSD_PIXELFORMAT) == 0 ||
         (header.flags & DDSD_CAPS) == 0)
   {
      LOGE("file is not a valid DDS");
      stream_close(f);
      return -1;
   }

   if ((header.pixel_format.flags & DDPF_FOURCC) == 0)
   {
      LOGE("fourcc is not specified");
      stream_close(f);
      return -1;
   }

   unsigned long block_bytes = 0;
   unsigned long format = IMAGE_RAW;

   if (header.pixel_format.fourcc == D3DFMT_DXT1)
   {
      format = IMAGE_COMPRESSED_RGBA_DXT1;
      block_bytes = 8;
   }
   else if (header.pixel_format.fourcc == D3DFMT_DXT3)
   {
      format = IMAGE_COMPRESSED_RGBA_DXT3;
      block_bytes = 16;
   }
   else if (header.pixel_format.fourcc == D3DFMT_DXT5)
   {
      format = IMAGE_COMPRESSED_RGBA_DXT5;
      block_bytes = 16;
   }
   else
   {
      LOGE("unsupported format: %04X", header.pixel_format.fourcc);
      return -1;
   }

   unsigned long l = 0;
   unsigned long mipmaps_data_size = 0;
   unsigned long nmipmaps = (header.flags & DDSD_MIPMAPCOUNT) ? header.nmipmaps : 1;
   unsigned long width = header.width;
   unsigned long height = header.height;

   for (l = 0; l < nmipmaps; ++l)
   {
      mipmaps_data_size += (max(4, width) >> 2) * (max(4, height) >> 2) * block_bytes;
      width /= 2;
      height /= 2;
   }

   image_t* image = (image_t*)malloc(sizeof(image_t) + nmipmaps * sizeof(mipmap_t) + mipmaps_data_size);
   image->format = format;
   image->bpp = header.depth;
   image->nmipmaps = nmipmaps;
   image->mipmaps = (mipmap_t*)((long)image + sizeof(image_t));

   unsigned long data_offset = (unsigned long)image->mipmaps + image->nmipmaps * sizeof(mipmap_t);

   width = header.width;
   height = header.height;
   mipmap_t* mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      mipmap->size = (max(4, width) >> 2) * (max(4, height) >> 2) * block_bytes;
      mipmap->width = width;
      mipmap->height = height;
      mipmap->data = (void*)data_offset;

      LOGI("mipmap #%ld: %ldx%ld %ld bytes", l, mipmap->width, mipmap->height, mipmap->size);

      stream_read(f, mipmap->data, mipmap->size);

      width /= 2;
      height /= 2;
      data_offset += mipmap->size;
   }

   (*pimage) = image;

   stream_close(f);
   return 0;
}

int main(int argc, const char** argv)
{
   if (argc < 2)
   {
      printf("usage: ./convert_dds file.dds\n");
      return 1;
   }

   int i = 0;
   for (i = 1; i < argc; ++i)
   {
      image_t* img = NULL;
      if (image_load_from_dds(&img, argv[i]) == 0)
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

