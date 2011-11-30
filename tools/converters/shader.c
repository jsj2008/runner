#include "shader.h"
#include <stdio.h>
#include <string.h>
#include <common.h>
#include <stream.h>

char* truncate_string(char* str)
{
   // get rid of spaces at the beginning of the string
   while (*str != '\0' && isspace(*str))
   {
      ++str;
   }

   // get rid of spaces at the end of the string
   char* tmp = str + strlen(str) - 1;
   while (str < tmp && isspace(*tmp))
   {
      *tmp = '\0';
      --tmp;
   }

   return str;
}

int isvariable(char c)
{
   switch (c)
   {
      case '_':
         return 1;
      default:
         return isalnum(c);
   }
}

char* remove_extra_spaces(char* str, char* result)
{
   int prev_token = -1;

   char prev = '\0';
   char* out = &result[0];
   while (*str != '\0')
   {
      char cur = *str;
      char next = *(str + 1);

      if (!isspace(cur) || (isvariable(prev) && isvariable(next)))
      {
         *out = cur;
         ++out;
      }

      ++str;
      prev = cur;
   }

   return result;
}

int shader_optimize(const char* fname_in, const char* fname_out)
{
   LOGI("Optimizing shader %s", fname_in);

   long size = 0;
   char* data = (char*)stream_read_file(fname_in, &size);
   if (data == NULL)
   {
      return -1;
   }

   enum {
      SHADER_VERTEX = 0,
      SHADER_PIXEL,
      SHADER_GEOMETRY,
      SHADER_TYPES_COUNT,
   } current_shader;

   current_shader = -1;
   char shaders[SHADER_TYPES_COUNT][4096] = {0};

   char* tmp = NULL;
   char* str = strtok_r(data, "\n", &tmp);
   while (str != NULL)
   {
      str = truncate_string(str);

      char* comment = strstr(str, "//");
      if (comment != NULL)
      {
         // string has a comment
         comment[0] = '\0';
         str = truncate_string(str);
      }

      if (strlen(str) > 0)
      {
         char tmp[1024] = {0};
         str = remove_extra_spaces(str, tmp);

         if (strcmp(str, "--vertex_shader") == 0)
         {
            current_shader = SHADER_VERTEX;
         }
         else if (strcmp(str, "--pixel_shader") == 0)
         {
            current_shader = SHADER_PIXEL;
         }
         else if (strcmp(str, "--geometry_shader") == 0)
         {
            current_shader = SHADER_GEOMETRY;
         }
         else if (current_shader != -1)
         {
            if (strstr(str, "precision") != str)
            {
               strcat(shaders[current_shader], str);
            }
         }
      }

      str = strtok_r(NULL, "\n", &tmp);
   }

   free(data);

   LOGI("Writing optimized shader to %s", fname_out);
   stream_t* f = NULL;
   if (stream_open_writer(&f, fname_out) != 0)
   {
      return -1;
   }

   int i = 0;
   const char delim[] = "\n\n";
   for (i = 0; i < SHADER_TYPES_COUNT; ++i)
   {
      LOGI(" shader #%d='%s'", i, shaders[i]);
      stream_write(f, shaders[i], strlen(shaders[i]));
      stream_write(f, delim, strlen(delim));
   }

   stream_close(f);

   return 0;
}

