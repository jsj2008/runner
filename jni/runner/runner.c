#include "common.h"
#include "gl.h"
#include "matrix.h"
#include "stream.h"
#include "camera.h"
#include "model.h"
#include "hlmdl.h"
#include "shader.h"
#include "octree.h"

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

GLuint gvTextureId = 0;
GLuint gvSkyboxTextureId = 0;

mat4f_t gModel;
cam_t camera;
model_t* mdl = NULL;
octree_t* level = NULL;
shader_t* model_shader;
shader_t* octree_shader;
shader_t* skybox_shader;
shader_t* bbox_shader;

float gAngle = 0.0f;
struct timeval prev_time;
long frames = 0;
long total_frames = 0;

vec4f_t* vec4(float x, float y, float z, float w)
{
   static vec4f_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   return &tmp;
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

int init(const char* apkPath)
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

   if (octree_load(&level, "assets/models/level.octree") != 0)
      return -1;

   if (model_load(&mdl, "assets/models/test.model") != 0)
      return -1;

   if (shader_load(&model_shader, "assets/shaders/test") != 0)
      return -1;

   if (shader_load(&octree_shader, "assets/shaders/level") != 0)
      return -1;

   if (shader_load(&skybox_shader, "assets/shaders/skybox") != 0)
      return -1;

   if (shader_load(&bbox_shader, "assets/shaders/bbox") != 0)
      return -1;

   gvSkyboxTextureId = create_texture("assets/textures/skybox.png");
   if (gvSkyboxTextureId == 0)
      return -1;

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   gvTextureId = create_texture("assets/textures/marble.png");
   if (gvTextureId == 0)
      return -1;

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   return 0;
}

void resize(int width, int height)
{
   LOGI("resize %dx%d", width, height);

   glViewport(0, 0, width, height);
   checkGLError("glViewport");

   glDepthRangef(0.1f, 1000.0f);
   checkGLError("glDepthRange");

   cam_init(&camera, 45.0f, (float)width/(float)height, 0.1f, 1000.0f);
   cam_set_pos(&camera, vec4(0.0f, 25.0f, 50.0f, 0.0f));
   cam_set_up(&camera, vec4(0.0f, 1.0f, 0.0f, 0.0f));
   cam_look_at(&camera, vec4(0.0f, 0.0f, 0.0f, 0.0f));
   cam_update(&camera);

   struct timezone tz;
   gettimeofday(&prev_time, &tz);
}

void update()
{

   int sampler_id = 0;

   shader_use(skybox_shader);
   shader_set_attrib_vertices(skybox_shader, "aPos", 3, GL_FLOAT, 0, skybox_vertices);
   shader_set_attrib_vertices(skybox_shader, "aTexCoord", 2, GL_FLOAT, 0, skybox_tex_coords);
   shader_set_attrib_vertices(skybox_shader, "aColor", 3, GL_FLOAT, 0, skybox_colors);
   shader_set_uniform_integers(skybox_shader, "uTex", 1, &sampler_id);

   glActiveTexture(GL_TEXTURE0);
   checkGLError("glActiveTexture");

   glBindTexture(GL_TEXTURE_2D, gvSkyboxTextureId);
   checkGLError("glBindTexture");

   glCullFace(GL_BACK);
   glDepthFunc(GL_ALWAYS);

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, skybox_indices);
   glDepthFunc(GL_LESS);

   //mat4_set_yrotation(&mdl->transform, DEG2RAD(gAngle));
   octree_draw(level, &camera);
   //model_render(mdl, &camera, total_frames);
   gAngle += 2.0f;

   ++total_frames;
   ++frames;

   struct timeval cur_time;
   struct timezone tz;
   gettimeofday(&cur_time, &tz);
   if (prev_time.tv_sec + 5 <= cur_time.tv_sec)
   {
      long diffs = cur_time.tv_sec - prev_time.tv_sec;
      long diffms = (cur_time.tv_usec - prev_time.tv_usec)/1000;
      long diff = diffs * 1000 + diffms;

      LOGI("Total: %4ld Frames: %4ld Diff: %ld FPS: %.2f", total_frames, frames, diff, (float)frames * 1000.0f / (float)diff);
      prev_time = cur_time;
      frames = 0;
   }
}

void scroll(long dt, float dx1, float dy1, float dx2, float dy2)
{
   static float speed = 1;

   float interval = (float)dt / 1000.0f;
   float coef = 0.05;//interval * speed;

   if (dx2 == 0.0f && dy2 == 0.0f)
   {
      // single finger sliding - strafe
      cam_slide(&camera, vec4(-coef * dx1, coef * dy1, 0.0f, 0.0f));

      int i = 0;
      for (i = 0; i < 4; ++i)
      {
         skybox_tex_coords[i*2] += coef * dx1 * 0.0025;
         skybox_tex_coords[i*2+1] -= coef * dy1 * 0.0025;
      }
   }
   else
   {
      // two finger sliding - move forward
      float v = coef * dy1;
      cam_slide(&camera, vec4(camera.view_dir.x * v, camera.view_dir.y * v, camera.view_dir.z * v, 0.0f));
   }

   cam_update(&camera);

   LOGI("Cam pos: [%.2f %.2f %.2f] dir: [%.2f %.2f %.2f]",
        camera.pos.x, camera.pos.y, camera.pos.z,
        camera.view_dir.x, camera.view_dir.y, camera.view_dir.z);
}

