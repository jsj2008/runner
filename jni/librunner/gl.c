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



