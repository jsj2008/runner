#include "stream.h"
#include "common.h"
#include <android/asset_manager.h>

struct stream_t
{
   AAsset* f;
};

AAssetManager* manager = NULL;

int stream_init(void* data)
{
   if (data == NULL)
   {
      return -1;
   }
   manager = (AAssetManager*)data;
   return 0;
}

int stream_open_reader(stream_t** pstream, const char* fname)
{
   if (pstream == NULL || fname == NULL)
   {
      return -1;
   }

   // remove leading slashes
   const char* path = fname;
   while ((*path == '/' || *path == '\\') && *path != '\0') ++path;

   AAsset* f = AAssetManager_open(manager, path, AASSET_MODE_BUFFER);
   if (f == NULL)
   {
      LOGE("Unable to open file for reading: %s", fname);
      return -1;
   }

   stream_t* stream = (stream_t*) malloc(sizeof(stream_t));
   stream->f = f;
   (*pstream) = stream;
   return 0;
}

int stream_open_writer(stream_t** pstream, const char* fname)
{
   LOGE("Not implemented");
   return -1;
}

void stream_close(stream_t* stream)
{
   if (stream != NULL)
   {
      AAsset_close(stream->f);
      free((void*) stream);
   }
}

long stream_size(stream_t* stream)
{
   if (stream == NULL)
   {
      return -1;
   }

   return AAsset_getLength(stream->f);
}

long stream_seek(stream_t* stream, long offset, int mode)
{
   if (stream == NULL)
   {
      return -1;
   }

   return AAsset_seek(stream->f, offset, mode);
}

long stream_read(stream_t* stream, void* buffer, long size)
{
   if (stream == NULL)
   {
      return -1;
   }

   return AAsset_read(stream->f, buffer, size);
}

long stream_write(stream_t* stream, const void* buffer, long size)
{
   LOGE("Not implemented");
   return -1;
}

void* stream_read_file(const char* fname, long* psize)
{
   stream_t* stream = NULL;
   if (stream_open_reader(&stream, fname) != 0)
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

