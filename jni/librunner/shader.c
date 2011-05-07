#include "shader.h"
#include "common.h"
#include "stream.h"
#include "gl.h"

#define MAX_SHADER_VARS 32

typedef struct shader_var_t
{
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

static GLuint load_shader_from_file(GLenum type, const char* fname)
{
   long size = 0;
   char* buf = (char*)stream_read_file(fname, &size);
   if (buf == NULL)
   {
      return 0;
   }

   buf[size - 1] = '\0';
   GLuint res = load_shader_from_string(type, buf);
   free(buf);

   return res;
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

static shader_var_t* add_var(shader_t* shader, const char* name, GLuint location)
{
   LOGI("Adding var %s location %d program %ld", name, location, shader->program);

   shader_var_t* var = &shader->vars[shader->nvars];
   ++shader->nvars;

   strcpy(var->name, name);
   var->location = location;
   return var;
}

static const shader_var_t* get_attrib_var(shader_t* shader, const char* name)
{
   const shader_var_t* var = find_var(shader, name);
   if (var == NULL)
   {
      GLuint location = glGetAttribLocation(shader->program, name);
      checkGLError("glGetAttribLocation");
      var = add_var(shader, name, location);
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
      var = add_var(shader, name, location);
   }
   return var;
}

int shader_load(shader_t** pshader, const char* name)
{
   char vs_name[256];
   strcpy(vs_name, name);
   strcat(vs_name, ".vs");
   LOGI("loading vertex shader");
   GLuint vs = load_shader_from_file(GL_VERTEX_SHADER, vs_name);
   if (vs == 0)
   {
      return -1;
   }

   char ps_name[256];
   strcpy(ps_name, name);
   strcat(ps_name, ".ps");
   LOGI("loading pixel shader");
   GLuint ps = load_shader_from_file(GL_FRAGMENT_SHADER, ps_name);
   if (ps == 0)
   {
      glDeleteShader(vs);
      checkGLError("glDeleteShader");
      return -1;
   }

   LOGI("creating shader program");
   GLuint program = glCreateProgram();
   if (program == 0)
   {
      LOGE("Unable to create program");
      return -1;
   }

   LOGI("attaching vertex shader");
   glAttachShader(program, vs);
   checkGLError("glAttachShader");

   LOGI("attaching pixel shader");
   glAttachShader(program, ps);
   checkGLError("glAttachShader");

   LOGI("linking shader program");
   glLinkProgram(program);
   checkGLError("glLinkProgram");

   LOGI("deleting no longer needed shaders");
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

void shader_set_attrib_vertices(shader_t* shader, const char* name, long components, long type, long stride, const void* values)
{
   const shader_var_t* var = get_attrib_var(shader, name);

   glVertexAttribPointer(var->location, components, type, GL_FALSE, stride, values);
   checkGLError("glVertexAttribPointer");

   glEnableVertexAttribArray(var->location);
   checkGLError("glEnableVertexAttribArray");
}

void shader_set_uniform_matrices(shader_t* shader, const char* name, long count, const float* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);

   glUniformMatrix4fv(var->location, count, GL_FALSE, values);
   checkGLError("glUniformMatrix4fv");
}

void shader_set_uniform_integers(shader_t* shader, const char* name, long count, const int* values)
{
   const shader_var_t* var = get_uniform_var(shader, name);

   glUniform1iv(var->location, count, values);
   checkGLError("glUniform1iv");
}

