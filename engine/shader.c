#include "shader.h"
#include "common.h"
#include "stream.h"
#include "gl_defs.h"

#define MAX_SHADER_VARS 32

typedef struct shader_var_t
{
   enum
   {
      VAR_ATTRIB = 0,
      VAR_UNIFORM,
   } type;

   char name[32];
   long location;
} shader_var_t;

struct shader_t
{
   char name[64];
   long program;

   long nvars;
   shader_var_t vars[MAX_SHADER_VARS];
};

static GLuint load_shader_from_string(GLenum type, const char* src)
{
   GLuint shader = glCreateShader(type);
   if (shader == 0)
   {
      LOGE("Unable to create shader");
      return 0;
   }

   glShaderSource(shader, 1, &src, NULL);
   checkGLError("glShaderSource");

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

static const shader_var_t* find_var(const shader_t* shader, const char* name)
{
   long i = 0;
   for (; i < shader->nvars; ++i)
   {
      if (strcmp(name, shader->vars[i].name) == 0)
      {
         return &shader->vars[i];
      }
   }
   return NULL;
}

static shader_var_t* add_var(shader_t* shader, const char* name, GLuint location, int type)
{
   LOGD("Adding var %s[%d] location %d program %ld", name, type, location, shader->program);

   shader_var_t* var = &shader->vars[shader->nvars];
   ++shader->nvars;

   strcpy(var->name, name);
   var->location = location;
   var->type = type;
   return var;
}

static const shader_var_t* get_attrib_var(shader_t* shader, const char* name)
{
   const shader_var_t* var = find_var(shader, name);
   if (var == NULL)
   {
      GLuint location = glGetAttribLocation(shader->program, name);
      checkGLError("glGetAttribLocation");
      var = add_var(shader, name, location, VAR_ATTRIB);
   }
   return var;
}

static const shader_var_t* get_uniform_var(shader_t* shader, const char* name)
{
   const shader_var_t* var = find_var(shader, name);
   if (var == NULL)
   {
      GLuint location = glGetUniformLocation(shader->program, name);
      checkGLError("glGetUniformLocation");
      var = add_var(shader, name, location, VAR_UNIFORM);
   }
   return var;
}

int shader_load(shader_t** pshader, const char* name)
{
   LOGI("Loading shader %s", name);

   long size = 0;
   char* buf = (char*)stream_read_file(name, &size);
   if (buf == NULL)
   {
      return -1;
   }

   char* shaders[3] = {0};
   char* begin = buf;
   int i = 0;
   while (begin != NULL && *begin != '\0' && i < 3)
   {
      shaders[i++] = begin;
      char* end = strstr(begin, "\n\n");
      if (end != NULL)
      {
         *end = '\0';
         begin = end + 2;
      }
      else
      {
         begin = NULL;
      }
      LOGD(" shader #%d: %s", i-1, shaders[i-1]);
   }

   GLuint vs = load_shader_from_string(GL_VERTEX_SHADER, shaders[0]);
   if (vs == 0)
   {
      return -1;
   }

   GLuint ps = load_shader_from_string(GL_FRAGMENT_SHADER, shaders[1]);
   if (ps == 0)
   {
      glDeleteShader(vs);
      checkGLError("glDeleteShader");
      return -1;
   }

   /*GLuint gs = load_shader_from_string(GL_GEOMETRY_SHADER, shaders[2]);
   if (gs == 0)
   {
      glDeleteShader(vs);
      checkGLError("glDeleteShader");
      glDeleteShader(ps);
      checkGLError("glDeleteShader");
      return -1;
   }*/

   GLuint program = glCreateProgram();
   if (program == 0)
   {
      LOGE("Unable to create program");
      return -1;
   }

   glAttachShader(program, vs);
   checkGLError("glAttachShader");

   glAttachShader(program, ps);
   checkGLError("glAttachShader");

   glLinkProgram(program);
   checkGLError("glLinkProgram");

   glDeleteShader(vs);
   checkGLError("glDeleteShader");

   glDeleteShader(ps);
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
      return -1;
   }

   shader_t* shader = (shader_t*)malloc(sizeof(shader_t));
   memset(shader, 0, sizeof(shader_t));
   strcpy(shader->name, name);
   shader->program = program;

   (*pshader) = shader;

   LOGI("Loaded shader program #%ld", shader->program);
   return 0;
}


void shader_free(shader_t* shader)
{
   if (shader->program != 0)
   {
      glDeleteProgram(shader->program);
      shader->program = 0;
   }
   free(shader);
}

void shader_use(const shader_t* shader)
{
   glUseProgram(shader->program);
   checkGLError("glUseProgram");
}

void shader_unuse(const shader_t* shader)
{
   long i = 0;
   for (; i < shader->nvars; ++i)
   {
      if (shader->vars[i].type == VAR_ATTRIB && shader->vars[i].location >= 0)
      {
         glDisableVertexAttribArray(shader->vars[i].location);
         checkGLError("glDisableVertexAttribArray");
      }
   }

   glUseProgram(0);
   checkGLError("glUseProgram");
}

void shader_set_attrib_vertices(shader_t* shader, const char* name, long components, long type, long stride, const void* values)
{
   const shader_var_t* var = get_attrib_var(shader, name);
   if (var->location < 0) return;

   glVertexAttribPointer(var->location, components, type, GL_FALSE, stride, values);
   checkGLError("glVertexAttribPointer");

   glEnableVertexAttribArray(var->location);
   checkGLError("glEnableVertexAttribArray");
}

void shader_set_uniform_matrices(shader_t* shader, const char* name, long count, const float* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);
   if (var->location < 0) return;

   glUniformMatrix4fv(var->location, count, GL_FALSE, values);
   checkGLError("glUniformMatrix4fv");
}

void shader_set_uniform_vectors(shader_t* shader, const char* name, long count, const float* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);
   if (var->location < 0) return;


   glUniform3fv(var->location, count, values);
   checkGLError("glUniform3iv");
}

void shader_set_uniform_integers(shader_t* shader, const char* name, long count, const int* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);
   if (var->location < 0) return;

   glUniform1iv(var->location, count, values);
   checkGLError("glUniform1iv");
}

void shader_set_uniform_floats(shader_t* shader, const char* name, long count, const float* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);
   if (var->location < 0) return;

   glUniform1fv(var->location, count, values);
   checkGLError("glUniform1fv");
}

