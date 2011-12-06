#include "image.h"
#include "stream.h"
#include "common.h"
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
   LOGI("Loading image from %s", fname);

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

   png_set_read_fn(png_ptr, (png_voidp)&p, png_read);
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
   int data_size = rowbytes * height;

   image_t* image = (image_t*)malloc(sizeof(image_t) + sizeof(mipmap_t) + data_size);
   image->format = IMAGE_RAW;
   image->bpp = channels * 8;
   image->nmipmaps = 1;
   image->mipmaps = (mipmap_t*)((unsigned char*)image + sizeof(image_t));
   mipmap_t* mipmap = &image->mipmaps[0];
   mipmap->size = data_size;
   mipmap->width = width;
   mipmap->height = height;
   mipmap->data = ((unsigned char*)image->mipmaps + sizeof(mipmap_t));

   png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
   int i = 0;
   for (i = 0; i < height; ++i)
   {
      row_pointers[height - i - 1] = (png_bytep)mipmap->data + i * rowbytes;
   }

   png_read_image(png_ptr, row_pointers);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   free(row_pointers);
   free(data);

   (*pimage) = image;

   return 0;
}

struct file_header_t
{
   char magic[8];
   long version;
   long data_offset;
   long data_size;
};

int image_load(image_t** pimage, const char* fname)
{
   LOGI("Loading image from %s", fname);

   stream_t* f = NULL;
   if (stream_open_reader(&f, fname) != 0)
   {
      return -1;
   }

   struct file_header_t header;
   stream_read(f, &header, sizeof(header));
   if (memcmp(header.magic, "RNNRTXTR", sizeof(header.magic)) != 0)
   {
      LOGE("Invalid file signature: %s", header.magic);
      stream_close(f);
      return -1;
   }

   long fsize = stream_size(f);

   if (header.data_offset + header.data_size < fsize)
   {
      LOGE("Invalid file size [offset: %ld size: %ld filesize: %ld]", header.data_offset, header.data_size, fsize);
      stream_close(f);
      return -1;
   }

   LOGD("Header [offset: %ld size: %ld]", header.data_offset, header.data_size);

   stream_seek(f, header.data_offset, SEEK_SET);
   char* data = malloc(header.data_size);
   stream_read(f, data, header.data_size);
   stream_close(f);

   image_t* image = (image_t*)data;
   image->mipmaps = (mipmap_t*)(data + (long)image->mipmaps);

   LOGD("Image '%s' [bpp %ld] format %d has %ld mipmaps", fname, image->bpp, image->format, image->nmipmaps);

   long l = 0;
   mipmap_t* mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      LOGD("\tMipmap [%ldx%ld] %ld bytes [offset %ld next %ld]", mipmap->width, mipmap->height, mipmap->size, (long)mipmap->data, mipmap->size + (long)mipmap->data);
      mipmap->data = (void*)(data + (long)mipmap->data);
   }
   (*pimage) = image;

   return 0;
}

#define WRITE(n)     { header.data_size += stream_write(f, &n, sizeof(n)); }
#define WRITEA(n, c) { header.data_size += stream_write(f, &n[0], c * sizeof(n[0])); }

int image_save(image_t* image, const char* fname)
{
   stream_t* f = NULL;
   if (stream_open_writer(&f, fname) != 0)
   {
      return -1;
   }

   struct file_header_t header =
   {
      .magic = "RNNRTXTR",
      .version = 1,
      .data_offset = sizeof(struct file_header_t),
      .data_size = 0,
   };

   stream_seek(f, header.data_offset, SEEK_SET);

   image_t img = (*image);
   img.mipmaps = (mipmap_t*)sizeof(image_t);
   WRITE(img);

   long offset = (long)img.mipmaps + img.nmipmaps * sizeof(mipmap_t);

   long l = 0;
   mipmap_t* mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      mipmap_t mip = (*mipmap);
      mip.data = (void*)offset;
      offset += mip.size;
      WRITE(mip);
   }

   mipmap = &image->mipmaps[0];
   for (l = 0; l < image->nmipmaps; ++l, ++mipmap)
   {
      WRITEA(((char*)mipmap->data), mipmap->size);
   }

   // write header
   stream_seek(f, 0, SEEK_SET);
   WRITE(header);

   stream_close(f);
   return 0;
}

void image_free(image_t* image)
{
   free(image);
}

