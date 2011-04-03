#include "gl.h"

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

GLuint gl_load_texture(tex2d_t* t)
{
   GLuint id = 0;

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   checkGLError("glPixelStorei");

   glGenTextures(1, &id);
   checkGLError("glGenTextures");

   glBindTexture(GL_TEXTURE_2D, id);
   checkGLError("glBindTexture");

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->width, t->height, 0, GL_RGB, GL_UNSIGNED_BYTE, t->data);
   checkGLError("glTexImage2D");

   LOGI("Loaded texture id = %d", id);

   return id;
}

GLuint gl_load_checkered_texture()
{
   unsigned char pixels[] =
   {
      127, 127, 127,
      0, 0, 0,
      0, 0, 0,
      127, 127, 127,
   };

   tex2d_t t;
   t.width = 2;
   t.height = 2;
   t.data = pixels;

   return gl_load_texture(&t);
}

