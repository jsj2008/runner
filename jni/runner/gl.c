#include "gl.h"
#include "assets.h"
#include <png.h>

void outGLString(const char* name, GLenum e)
{
   const char* str = (const char*)glGetString(e);
   LOGI("GL %s = %s", name, str);
}

void checkGLError(const char* op)
{
   GLint error = glGetError();
   for (; error != 0; error = glGetError())
   {
      LOGI("%s caused glerror #%d", op, error);
   }
}

static GLuint loadShader(GLenum type, const char* src)
{
   LOGI("creating shader");
   GLuint shader = glCreateShader(type);
   if (shader == 0)
   {
      LOGE("Unable to create shader");
      return 0;
   }


   int len = strlen(src);
   LOGI("setting shader source [%d bytes]: %s", len, src);
   glShaderSource(shader, 1, &src, NULL);
   checkGLError("glShaderSource");

   LOGI("compiling shader");
   glCompileShader(shader);
   checkGLError("glCompileShader");

   GLint compiled = GL_FALSE;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
   if (compiled != GL_TRUE)
   {
      LOGE("Not compiled: %d", compiled);
      GLint len = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
      LOGE("Info length: %d", len);
      if (len > 0)
      {
         char* buf = (char*)malloc(len);
         glGetShaderInfoLog(shader, len, NULL, buf);
         LOGE("Unable to compile shader: %s", buf);
         free(buf);
      }
      glDeleteShader(shader);
      return 0;
   }

   return shader;
}

GLuint createShaderProgram(const char* vertexShaderSrc, const char* pixelShaderSrc)
{
   if (vertexShaderSrc == NULL || pixelShaderSrc == NULL)
   {
      LOGE("Invalid arguments");
      return 0;
   }

   LOGI("loading vertex shader");
   GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSrc);
   if (vertexShader == 0)
   {
      LOGE("Unable to load vertex shader");
      return 0;
   }

   LOGI("loading pixel shader");
   GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pixelShaderSrc);
   if (pixelShader == 0)
   {
      LOGE("Unable to load pixel shader");
      return 0;
   }

   LOGI("creating shader program");
   GLuint program = glCreateProgram();
   if (program == 0)
   {
      LOGE("Unable to create program");
      return 0;
   }

   LOGI("attaching vertex shader");
   glAttachShader(program, vertexShader);
   checkGLError("glAttachShader");

   LOGI("attaching pixel shader");
   glAttachShader(program, pixelShader);
   checkGLError("glAttachShader");

   LOGI("linking shader program");
   glLinkProgram(program);
   checkGLError("glLinkProgram");

   LOGI("deleting no longer needed shaders");
   glDeleteShader(vertexShader);
   checkGLError("glDeleteShader");

   glDeleteShader(pixelShader);
   checkGLError("glDeleteShader");

   GLint linkStatus = GL_FALSE;
   glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
   if (linkStatus == GL_FALSE)
   {
      GLint len = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
      if (len > 0)
      {
         char* buf = (char*)malloc(len);
         glGetProgramInfoLog(program, len, NULL, buf);
         LOGE("Unable to link program: %s", buf);
         free(buf);
      }
      glDeleteProgram(program);
      return 0;
   }

   return program;
}


typedef struct png_read_data_t
{
   char* data;
   int offset;
} png_read_data_t;

void png_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_read_data_t* p = (png_read_data_t*)png_get_io_ptr(png_ptr);

   LOGI("png_read: %d total: %d", length, p->offset);

   memcpy(data, p->data + p->offset, length);
   p->offset += length;
}

GLuint createTexture(const char* fname, int* width, int* height)
{
   int size = 0;
   char* data = readFile(fname, &size);
   if (data == NULL)
      return 0;

   if (png_sig_cmp(data, 0, 8) != 0)
   {
      LOGE("File %s is not PNG", fname);
      free(data);
      return 0;
   }

   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL)
   {
      LOGE("Could not initialize png read struct");
      return 0;
   }

   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      LOGE("Could not initialize png info struct");
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return 0;
   }

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      LOGE("An error occured while reading the PNG file");
      return 0;
   }

   png_read_data_t p;
   p.data = data;
   p.offset = 0;

   png_set_read_fn(png_ptr, (voidp)&p, png_read);
   png_read_info(png_ptr, info_ptr);

   int _width = png_get_image_width(png_ptr, info_ptr);
   int _height = png_get_image_height(png_ptr, info_ptr);
   int color_type = png_get_color_type(png_ptr, info_ptr);
   int channels = png_get_channels(png_ptr, info_ptr);
   int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

   LOGI("Reading image [%dx%d:%d] color type %d channels %d", _width, _height, bit_depth, color_type, channels);

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
      +channels;
   }

   if (bit_depth == 16)
   {
      png_set_strip_16(png_ptr);
   }

   png_read_update_info(png_ptr, info_ptr);
   const unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
   char* img = (char*)malloc(rowbytes * _height);

   png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * _height);
   int i = 0;
   for (i = 0; i < _height; ++i)
   {
      row_pointers[_height - i - 1] = (png_bytep)img + i * rowbytes;
   }

   png_read_image(png_ptr, row_pointers);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   free(row_pointers);
   free(data);

   GLuint id = 0;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
   checkGLError("glTexImage2D");

   free(img);

   LOGI("Loaded texture id = %d", id);

   return id;
}

GLuint createCheckeredTexture()
{
   GLuint id = 0;

   char pixels[] =
   {
      127, 127, 127,
      0, 0, 0,
      0, 0, 0,
      127, 127, 127,
   };

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
   checkGLError("glTexImage2D");

   LOGI("Checkered texture id = %d", id);
   return id;
}

