#include "stream.h"
#include <zip.h>

struct stream_t
{
   struct zip_file* zip;
   struct zip_stat stat;
};

struct zip* g_archive = NULL;

int stream_set_root(const char* root)
{
   if (g_archive != NULL)
   {
      LOGI("Closing APK file");
      zip_close(g_archive);
      g_archive = NULL;
   }

   LOGI("Opening APK file %s", root);
   g_archive = zip_open(root, 0, NULL);
   if (g_archive == NULL)
   {
      LOGE("Unable to open APK archive");
      return -1;
   }

   int i = 0;
   int nfiles = zip_get_num_files(g_archive);
   for (i = 0; i < nfiles; ++i)
   {
      const char* fname = zip_get_name(g_archive, i, 0);
      if (fname == NULL)
      {
         LOGE("Unable to read filename #%d: %s", i, zip_strerror(g_archive));
         return -1;
      }
      LOGI("File #%i: %s", i, fname);
   }
   return 0;
}

int stream_open_reader(stream_t** pstream, const char* fname)
{
   if (g_archive == NULL)
   {
      LOGE("APK archive is not opened");
      return -1;
   }

   if (pstream == NULL || fname == NULL)
   {
      return -1;
   }

   stream_t* stream = (stream_t*)malloc(sizeof(stream_t));

   if (zip_stat(g_archive, fname, 0, &stream->stat))
   {
      LOGE("Unable to stat file %s: %s", fname, zip_strerror(g_archive));
      free(stream);
      return -1;
   }

   stream->zip = zip_fopen(g_archive, fname, 0);
   if (stream->zip == NULL)
   {
      LOGE("Unable to open file %s: %s", fname, zip_strerror(g_archive));
      free(stream);
      return -1;
   }

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
      zip_fclose(stream->zip);
      free((void*)stream);
   }
}

long stream_size(stream_t* stream)
{
   if (stream == NULL)
   {
      return -1;
   }

   return stream->stat.size;
}

long stream_seek(stream_t* stream, long offset, int mode)
{
   LOGE("Not implemented");
   return -1;
}

long stream_read(stream_t* stream, void* buffer, long size)
{
   if (stream == NULL)
   {
      return -1;
   }

   return zip_fread(stream->zip, buffer, size);
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
   void* buffer = malloc(size);

   (*psize) = stream_read(stream, buffer, size);

   stream_close(stream);

   return buffer;
}

