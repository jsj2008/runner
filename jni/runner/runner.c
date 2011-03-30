#include "common.h"
#include "gl.h"
#include "matrix.h"
#include "assets.h"
#include "camera.h"

static GLfloat skybox_vertices[] =
{
   -1.0f, -1.0f, 1.0f,
   1.0f, -1.0f, 1.0f,
   1.0f, 1.0f, 1.0f,
   -1.0f, 1.0f, 1.0f,
};

static GLfloat skybox_colors[] =
{
   0.5f, 0.0f, 0.0f,
   0.5f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 0.0f,
};

static GLubyte skybox_indices[] =
{
   0, 1, 2,
   0, 2, 3,
};

static GLfloat skybox_tex_coords[] =
{
   0.25f, 0.1f,
   0.75f, 0.1f,
   0.75f, 0.9f,
   0.25f, 0.9f,
};

static GLfloat vertices[] =
{
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   0.5f, -0.5f, 0.5f,
   0.5f, -0.5f, -0.5f,
   -0.5f, 0.5f, -0.5f,
   -0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, -0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, 0.5f, -0.5f,
   0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   -0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   0.5f, -0.5f, 0.5f,
   -0.5f, -0.5f, -0.5f,
   -0.5f, -0.5f, 0.5f,
   -0.5f, 0.5f, 0.5f,
   -0.5f, 0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, 0.5f,
   0.5f, 0.5f, 0.5f,
   0.5f, 0.5f, -0.5f
};

static GLfloat texCoords[] =
{
   0.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f,
   1.0f, 0.0f,
   1.0f, 0.0f,
   1.0f, 1.0f,
   0.0f, 1.0f,
   0.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f,
   1.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f,
   1.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f,
   1.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 1.0f,
   1.0f, 1.0f,
   1.0f, 0.0f
};
static GLfloat normals[] =
{
   0.0f, -1.0f, 0.0f,
   0.0f, -1.0f, 0.0f,
   0.0f, -1.0f, 0.0f,
   0.0f, -1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 1.0f, 0.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, -1.0f,
   0.0f, 0.0f, 1.0f,
   0.0f, 0.0f, 1.0f,
   0.0f, 0.0f, 1.0f,
   0.0f, 0.0f, 1.0f,
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f
};

static GLubyte indices[] =
{
   0, 2, 1,
   0, 3, 2,
   4, 5, 6,
   4, 6, 7,
   8, 9, 10,
   8, 10, 11,
   12, 15, 14,
   12, 14, 13,
   16, 17, 18,
   16, 18, 19,
   20, 23, 22,
   20, 22, 21
};

GLuint gFrames = 0;
GLuint gModelProgram = 0;
GLuint gvPosHandle = 0;
GLuint gvTexCoordHandle = 0;
GLuint gvNormalHandle = 0;
GLuint gvSampler = 0;
GLuint gvMVP = 0;
GLuint gvTextureId = 0;

GLuint gvSkyboxPos = 0;
GLuint gvSkyboxTexCoord = 0;
GLuint gvSkyboxColor = 0;
GLuint gvSkyboxSampler = 0;
GLuint gvSkyboxTextureId = 0;
GLuint gSkyboxProgram = 0;


mat4f_t gModel;
cam_t camera;

vec4f_t* vec4(float x, float y, float z, float w)
{
   static vec4f_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   return &tmp;
}

void init(const char* apkPath)
{
   LOGI("init");

   if (setAPKPath(apkPath) != 0)
   {
      LOGE("Error setting APK path");
      return;
   }

   outGLString("Version", GL_VERSION);
   outGLString("Vendor", GL_VENDOR);
   outGLString("Renderer", GL_RENDERER);
   outGLString("Extensions", GL_EXTENSIONS);

   glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
}


GLuint load_program(const char* vs_fname, const char* ps_fname)
{
   int size = 0;

   char* vertexShader = readFile(vs_fname, &size);
   vertexShader[size - 1] = '\0';

   char* pixelShader = readFile(ps_fname, &size);
   pixelShader[size - 1] = '\0';

   GLuint program = createShaderProgram(vertexShader, pixelShader);

   free(vertexShader);
   free(pixelShader);

   if (program == 0)
   {
      LOGE("Unable to create program");
   }

   return program;
}

void resize(int width, int height)
{
   LOGI("resize %dx%d", width, height);

   gModelProgram = load_program("assets/shaders/test.vs", "assets/shaders/test.ps");
   if (gModelProgram == 0)
      return;

   gSkyboxProgram = load_program("assets/shaders/skybox.vs", "assets/shaders/skybox.ps");
   if (gSkyboxProgram == 0)
      return;

   gvSkyboxPos = glGetAttribLocation(gSkyboxProgram, "aPos");
   gvSkyboxTexCoord = glGetAttribLocation(gSkyboxProgram, "aTexCoord");
   gvSkyboxColor = glGetAttribLocation(gSkyboxProgram, "aColor");
   gvSkyboxSampler = glGetUniformLocation(gSkyboxProgram, "uTex");
   gvSkyboxTextureId = createTexture("assets/textures/skybox.png", NULL, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   gvTextureId = createTexture("assets/textures/marble.png", NULL, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   gvPosHandle = glGetAttribLocation(gModelProgram, "aPos");
   checkGLError("glGetAttribLocation");

   gvTexCoordHandle = glGetAttribLocation(gModelProgram, "aTexCoord");
   checkGLError("glGetAttribLocation");

   gvNormalHandle = glGetAttribLocation(gModelProgram, "aNormal");
   checkGLError("glGetAttribLocation");

   gvSampler = glGetUniformLocation(gModelProgram, "uTex");
   checkGLError("glGetUniformLocation");

   gvMVP = glGetUniformLocation(gModelProgram, "uMVP");
   checkGLError("glGetUniformLocation");

   glViewport(0, 0, width, height);
   checkGLError("glViewport");

   glDepthRangef(0.1f, 1000.0f);
   checkGLError("glDepthRange");

   cam_init(&camera, 45.0f, (float)width/(float)height, 0.1f, 1000.0f);
   cam_set_pos(&camera, vec4(0.0f, 0.0f, 3.0f, 0.0f));
   cam_set_up(&camera, vec4(0.0f, 1.0f, 0.0f, 0.0f));
   cam_look_at(&camera, vec4(0.0f, 0.0f, 0.0f, 0.0f));
   cam_update(&camera);
}

float gAngle = 0.0f;

void update()
{
   if (gModelProgram == 0 || gSkyboxProgram == 0)
      return;

   gAngle += 2.0f;
   mat4_set_xrotation(&gModel, DEG2RAD(gAngle));

   mat4f_t mv;
   mat4f_t mvp;
   mat4_mult(&mv, &camera.view, &gModel);
   mat4_mult(&mvp, &camera.proj, &mv);

   glDepthFunc(GL_ALWAYS);
   glUseProgram(gSkyboxProgram);

   glVertexAttribPointer(gvSkyboxPos, 3, GL_FLOAT, GL_FALSE, 0, skybox_vertices);
   glVertexAttribPointer(gvSkyboxTexCoord, 2, GL_FLOAT, GL_FALSE, 0, skybox_tex_coords);
   glVertexAttribPointer(gvSkyboxColor, 3, GL_FLOAT, GL_FALSE, 0, skybox_colors);

   glEnableVertexAttribArray(gvSkyboxPos);
   glEnableVertexAttribArray(gvSkyboxTexCoord);
   glEnableVertexAttribArray(gvSkyboxColor);

   glActiveTexture(GL_TEXTURE0);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, gvSkyboxTextureId);
   checkGLError("glBindTexture");

   glUniform1i(gvSkyboxSampler, 0);
   checkGLError("glUniform1i");

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, skybox_indices);
   glDepthFunc(GL_LESS);


   glUseProgram(gModelProgram);
   checkGLError("glUseProgram");

   glUniformMatrix4fv(gvMVP, 1, GL_FALSE, mat4_data(&mvp));
   checkGLError("glUniformMatrix4fv");

   glVertexAttribPointer(gvPosHandle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
   checkGLError("glVertexAttribPointer");

   glVertexAttribPointer(gvTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
   checkGLError("glVertexAttribPointer");

   glVertexAttribPointer(gvNormalHandle, 3, GL_FLOAT, GL_FALSE, 0, normals);
   checkGLError("glVertexAttribPointer");

   glEnableVertexAttribArray(gvPosHandle);
   checkGLError("glEnableVertexAttribArray");

   glEnableVertexAttribArray(gvTexCoordHandle);
   checkGLError("glEnableVertexAttribArray");

   glEnableVertexAttribArray(gvNormalHandle);
   checkGLError("glEnableVertexAttribArray");

   glActiveTexture(GL_TEXTURE0);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, gvTextureId);
   checkGLError("glBindTexture");

   glUniform1i(gvSampler, 0);
   checkGLError("glUniform1i");

   glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
   checkGLError("glDrawElements");

   ++gFrames;
}

void scroll(long delta_time, float delta_x, float delta_y)
{
   static float speed = 1;

   float interval = (float)delta_time / 1000.0f;
   float coef = interval * speed;

   cam_slide(&camera, vec4(-coef * delta_x, coef * delta_y, 0.0f, 0.0f));
   cam_update(&camera);

   int i = 0;
   for (i = 0; i < 4; ++i)
   {
      skybox_tex_coords[i*2] += interval * delta_x * 0.1;
      skybox_tex_coords[i*2+1] -= interval * delta_y * 0.1;
   }
}

