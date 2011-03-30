#include <string.h>
#include <zip.h>
#include "assets.h"
#include "common.h"

struct zip* gArchive = NULL;

int setAPKPath(const char* path)
{
   if (gArchive != NULL)
   {
      LOGI("Closing APK file");
      zip_close(gArchive);
      gArchive = NULL;
   }

   LOGI("Opening APK file %s", path);
   gArchive = zip_open(path, 0, NULL);
   if (gArchive == NULL)
   {
      LOGE("Unable to open APK archive");
      return 1;
   }

   int i = 0;
   int nfiles = zip_get_num_files(gArchive);
   for (i = 0; i < nfiles; ++i)
   {
      const char* fname = zip_get_name(gArchive, i, 0);
      if (fname == NULL)
      {
         LOGE("Unable to read filename #%d: %s", i, zip_strerror(gArchive));
         return 1;
      }
      LOGI("File #%i: %s", i, fname);
   }
   return 0;
}

char* readFile(const char* fname, int* size)
{
   struct zip_stat s;
   if (zip_stat(gArchive, fname, 0, &s) != 0)
   {
      LOGE("Unable to stat file %s: %s", fname, zip_strerror(gArchive));
      return NULL;
   }

   struct zip_file* f = zip_fopen(gArchive, fname, 0);
   if (f == NULL)
   {
      LOGE("Unable to open file %s: %s", fname, zip_strerror(gArchive));
      return NULL;
   }

   char* buf = (char*)malloc(s.size);
   int read = zip_fread(f, buf, s.size);
   if (read >= 0)
   {
      LOGI("Read %d bytes", read);
      if (size != NULL)
      {
         (*size) = s.size;
      }
   }
   else
   {
      LOGE("Unable to read %d bytes from file %s: %s", s.size, fname, zip_strerror(gArchive));
      zip_fclose(f);
      free(buf);
      buf = NULL;
   }

   zip_fclose(f);
   return buf;
}

