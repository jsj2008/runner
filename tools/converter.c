#include <getopt.h>
#include <stdio.h>
#include <common.h>
#include <image.h>
#include "converters/dds.h"
#include "converters/pvr.h"
#include "converters/shader.h"

int main(int argc, char** argv)
{
   static struct option long_options[] =
   {
      {"input",   required_argument, 0, 'i'},
      {"output",  required_argument, 0, 'o'},
      {0, 0, 0, 0}
   };

   int option_index = 0;
   char c = 0;

   char fin[256] = {0};
   char fout[256] = {0};
   while (1)
   {
      c = getopt_long (argc, argv, "i:o:", long_options, &option_index);
      if (c == -1)
      {
         break;
      }

      switch (c)
      {
         case 0:
            LOGI("option_index: %d", option_index);
            break;
         case 'i':
            strcpy(fin, optarg);
            break;
         case 'o':
            strcpy(fout, optarg);
            break;
      }
   }

   if (strlen(fin) == 0)
   {
      LOGE("You should specify input file name");
      return -1;
   }

   const char* ext = strrchr(fin, '.');
   if (ext == NULL)
   {
      LOGE("Input file without extension. Do not know what to do");
      return -1;
   }

   if (strlen(fout) == 0)
   {
      strcpy(fout, fin);
      char* end = strrchr(fout, '.');
      if (end != NULL)
      {
         strcpy(end, ".texture");
      }
   }

   image_t* img = NULL;
   if (strcasecmp(ext, ".dds") == 0)
   {
      image_load_from_dds(&img, fin);
   }
   else if (strcasecmp(ext, ".pvr") == 0)
   {
      image_load_from_pvr(&img, fin);
   }
   else if (strcasecmp(ext, ".shader") == 0)
   {
      shader_optimize(fin, fout);
   }
   else
   {
      LOGE("Unknown file extension: %s", ext);
      return -1;
   }

   if (img != NULL)
   {
      LOGI("Saving image to %s", fout);
      image_save(img, fout);
      image_free(img);
   }

   return 0;
}

