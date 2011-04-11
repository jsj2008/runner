#include "common.h"
#include "gl.h"
#include "matrix.h"
#include "stream.h"
#include "camera.h"
#include "model.h"
#include "hlmdl.h"
#include "shader.h"

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
GLuint gvTextureId = 0;
GLuint gvSkyboxTextureId = 0;

mat4f_t gModel;
cam_t camera;
model_t* mdl = NULL;
shader_t model_shader;
shader_t skybox_shader;

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

   if (stream_set_root(apkPath) != 0)
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

   if (model_load(&mdl, "assets/models/big_rock.mdl") != 0)
   {
      LOGE("Unable to load model\n");
   }
}

GLuint create_texture(const char* fname)
{
   tex2d_t t;
   if (tex2d_load_from_png(&t, fname) != 0)
   {
      LOGE("Unable to load texture from %s", fname);
      return 0;
   }

   GLuint texId = gl_load_texture(&t);
   if (texId == 0)
   {
      LOGE("Unable to load texture to GPU");
      return 0;
   }

   return texId;
}

void resize(int width, int height)
{
   LOGI("resize %dx%d", width, height);

   if (shader_load(&model_shader, "assets/shaders/test") == 0)
      return;

   if (shader_load(&skybox_shader, "assets/shaders/skybox") == 0)
      return;

   gvSkyboxTextureId = create_texture("assets/textures/skybox.png");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   gvTextureId = create_texture("assets/textures/marble.png");
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glViewport(0, 0, width, height);
   checkGLError("glViewport");

   glDepthRangef(0.1f, 1000.0f);
   checkGLError("glDepthRange");

   cam_init(&camera, 45.0f, (float)width/(float)height, 0.1f, 1000.0f);
   cam_set_pos(&camera, vec4(0.0f, 0.0f, 50.0f, 0.0f));
   cam_set_up(&camera, vec4(0.0f, 1.0f, 0.0f, 0.0f));
   cam_look_at(&camera, vec4(0.0f, 0.0f, 0.0f, 0.0f));
   cam_update(&camera);
}

float gAngle = 0.0f;

void draw_mesh(cam_t* c, mesh_t* m)
{
   mat4f_t mv;
   mat4f_t mvp;
   mat4_mult(&mv, &c->view, &gModel);
   mat4_mult(&mvp, &c->proj, &mv);

   int sampler_id = 0;
   shader_use(&model_shader);
   shader_set_uniform_matrices(&model_shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(&model_shader, "aPos", 3, GL_FLOAT, sizeof(vertex_t), &m->vertices[0].pos);
   shader_set_attrib_vertices(&model_shader, "aTexCoord", 2, GL_FLOAT, sizeof(vertex_t), &m->vertices[0].tex_coord);
   shader_set_attrib_vertices(&model_shader, "aNormal", 3, GL_FLOAT, sizeof(vertex_t), &m->vertices[0].normal);
   shader_set_uniform_integers(&model_shader, "uTex", 1, &sampler_id);

   glActiveTexture(GL_TEXTURE0 + sampler_id);
   glBindTexture(GL_TEXTURE_2D, gvTextureId);

   glCullFace(GL_FRONT);
   glDrawElements(GL_TRIANGLES, m->nindices, GL_UNSIGNED_INT, m->indices);
}

void update()
{
   gAngle += 2.0f;
   mat4_set_xrotation(&gModel, DEG2RAD(gAngle));

   mat4f_t mv;
   mat4f_t mvp;
   mat4_mult(&mv, &camera.view, &gModel);
   mat4_mult(&mvp, &camera.proj, &mv);

   glCullFace(GL_BACK);
   glDepthFunc(GL_ALWAYS);

   int sampler_id = 0;

   shader_use(&skybox_shader);
   shader_set_attrib_vertices(&skybox_shader, "aPos", 3, GL_FLOAT, 0, skybox_vertices);
   shader_set_attrib_vertices(&skybox_shader, "aTexCoord", 2, GL_FLOAT, 0, skybox_tex_coords);
   shader_set_attrib_vertices(&skybox_shader, "aColor", 3, GL_FLOAT, 0, skybox_colors);
   shader_set_uniform_integers(&skybox_shader, "uTex", 1, &sampler_id);

   glActiveTexture(GL_TEXTURE0);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, gvSkyboxTextureId);
   checkGLError("glBindTexture");

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, skybox_indices);
   glDepthFunc(GL_LESS);

   model_render(mdl, &camera);

   ++gFrames;
}

void scroll(long delta_time, float delta_x, float delta_y)
{
   static float speed = 1;

   float interval = (float)delta_time / 1000.0f;
   float coef = 1.0f;//interval * speed;

   //cam_slide(&camera, vec4(-coef * delta_x, coef * delta_y, 0.0f, 0.0f));
   cam_slide(&camera, vec4(0.0f, 0.0f, coef * delta_y, 0.0f));
   cam_update(&camera);
   /*
      int i = 0;
      for (i = 0; i < 4; ++i)
      {
         skybox_tex_coords[i*2] += interval * delta_x * 0.1;
         skybox_tex_coords[i*2+1] -= interval * delta_y * 0.1;
      }
   */
}

