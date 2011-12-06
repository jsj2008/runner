#include "stream.h"
#include "common.h"

struct stream_t
{
   FILE* f;
};

static char root[256];

int stream_init(void* data)
{
   strcpy(root, data);
   return 0;
}

int stream_open_reader(stream_t** pstream, const char* fname)
{
   if(pstream == NULL || fname == NULL)
   {
      return -1;
   }

   char path[256] = {0};
   strcpy(path, root);
   strcat(path, fname);

   FILE* f = fopen(path, "rb");
   if(f == NULL)
   {
      LOGE("Unable to open file for reading: %s", path);
      return -1;
   }

   stream_t* stream = (stream_t*) malloc(sizeof(stream_t));
   stream->f = f;
   (*pstream) = stream;
   return 0;
}

int stream_open_writer(stream_t** pstream, const char* fname)
{
   if(pstream == NULL || fname == NULL)
   {
      return -1;
   }

   char path[256] = {0};
   strcpy(path, root);
   strcat(path, fname);

   FILE* f = fopen(path, "wb");
   if(f == NULL)
   {
      LOGE("Unable to open file for writing: %s", path);
      return -1;
   }

   stream_t* stream = (stream_t*) malloc(sizeof(stream));
   stream->f = f;
   (*pstream) = stream;
   return 0;
}

void stream_close(stream_t* stream)
{
   if(stream != NULL)
   {
      fclose(stream->f);
      free((void*) stream);
   }
}

long stream_size(stream_t* stream)
{
   if(stream == NULL)
   {
      return -1;
   }

   long old_pos = fseek(stream->f, 0, SEEK_END);
   long size = ftell(stream->f);
   fseek(stream->f, old_pos, SEEK_SET);
   return size;
}

long stream_seek(stream_t* stream, long offset, int mode)
{
   if(stream == NULL)
   {
      return -1;
   }

   return fseek(stream->f, offset, mode);
}

long stream_read(stream_t* stream, void* buffer, long size)
{
   if(stream == NULL)
   {
      return -1;
   }

   return fread(buffer, 1, size, stream->f);
}

long stream_write(stream_t* stream, const void* buffer, long size)
{
   if(stream == NULL)
   {
      return -1;
   }

   return fwrite(buffer, 1, size, stream->f);
}

void* stream_read_file(const char* fname, long* psize)
{
   stream_t* stream = NULL;
   if(stream_open_reader(&stream, fname) != 0)
   {
      return NULL;
   }

   long size = stream_size(stream);
   void* buffer = malloc(size + 1);
   memset(buffer, 0, size + 1);

   (*psize) = stream_read(stream, buffer, size);

   stream_close(stream);

   return buffer;
}

