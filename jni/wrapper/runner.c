#include "runner.h"
#include "common.h"
#include "gl.h"
#include "math.h"
#include "stream.h"
#include "camera.h"
#include "model.h"
#include "hlmdl.h"
#include "shader.h"
#include "octree.h"
#include "material.h"
#include "resman.h"
#include "entity.h"

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

cam_t camera;
octree_t* level = NULL;
resman_t* g_resman = NULL;

entity_t entities[16];
long nentities = sizeof(entities)/sizeof(entities[0]);

vec3f_t* vec3(float x, float y, float z)
{
   static vec3f_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   return &tmp;
}

quat_t* quat(float x, float y, float z, float w)
{
   static quat_t tmp;
   tmp.x = x;
   tmp.y = y;
   tmp.z = z;
   tmp.w = w;
   return &tmp;
}

long frames = 0;
long total_frames = 0;
struct timeval prev_time;
struct timeval fps_time;

void timers_init()
{
   frames = 0;
   total_frames = 0;

   struct timezone tz;
   gettimeofday(&fps_time, &tz);
   prev_time = fps_time;
}

float timers_update()
{
   ++total_frames;
   ++frames;

   struct timeval cur_time;
   struct timezone tz;
   gettimeofday(&cur_time, &tz);
   float dt = (cur_time.tv_sec - prev_time.tv_sec) + (cur_time.tv_usec - prev_time.tv_usec) / 1000000.0f;
   prev_time = cur_time;

   if (fps_time.tv_sec + 5 <= cur_time.tv_sec)
   {
      long diffs = cur_time.tv_sec - fps_time.tv_sec;
      long diffms = (cur_time.tv_usec - fps_time.tv_usec)/1000;
      long diff = diffs * 1000 + diffms;

      LOGI("Total: %4ld Frames: %4ld Diff: %ld FPS: %.2f", total_frames, frames, diff, (float)frames * 1000.0f / (float)diff);
      fps_time = cur_time;
      frames = 0;
   }
   return dt;
}

int init(const char* apkPath)
{
   LOGI("init");

   if (stream_set_root(apkPath) != 0)
   {
      LOGE("Error setting APK path");
      return -1;
   }

   if (resman_init(&g_resman) != 0)
      return -1;

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

   level = resman_get_octree(g_resman, "assets/models/level.octree");
   if (level == NULL)
      return -1;

   int i = 0;
   entity_t* e = &entities[0];
   for (i = 0; i < nentities; ++i, ++e)
   {
      strcpy(e->model, "assets/models/crate.model");
      e->phys.position = *vec3(4.0f * i, 2.0f, 0.0f);
      e->phys.orientation = *quat(0.0f, 0.0f, 0.0f, 1.0f);
      e->phys.velocity = *vec3(0.0f, 0.0f, 0.0f);
      e->phys.angular_velocity = *vec3(0.0f, 0.0f, 0.0f);
      e->phys.mass = 1.0f;
      physent_set_box_inertia(&e->phys, 1.0f, 1.0f, 1.0f);
   }

   return 0;
}

void shutdown()
{
   LOGI("shutdown");

   if (g_resman != NULL)
   {
      resman_show(g_resman);

      LOGI("Releasing old resources");
      resman_free(g_resman);
      g_resman = NULL;
   }
}

void resize(int width, int height)
{
   LOGI("resize %dx%d", width, height);

   glViewport(0, 0, width, height);
   checkGLError("glViewport");

   glDepthRangef(0.1f, 1000.0f);
   checkGLError("glDepthRange");

   cam_init(&camera, 45.0f, (float)width/(float)height, 0.1f, 1000.0f);
   cam_set_pos(&camera, vec3(0.0f, 25.0f, 50.0f));
   cam_set_up(&camera, vec3(0.0f, 1.0f, 0.0f));
   cam_look_at(&camera, vec3(0.0f, 0.0f, 0.0f));
   camera.max_speed = *vec3(50.0f, 50.0f, 50.0f);
   camera.acceleration = *vec3(1000.0f, 1000.0f, 1000.0f);
   camera.decceleration = *vec3(25.0f, 25.0f, 25.0f);
   camera.target = camera.pos;
   cam_update(0.0f, &camera);

   timers_init();
}

void draw_skybox()
{
   material_t* mtl = resman_get_material(g_resman, "assets/materials/skybox.material");
   if (mtl == NULL)
      return;

   shader_t* shader = material_get_shader(mtl);
   if (shader == NULL)
      return;

   material_use(mtl, 0);
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, 0, skybox_vertices);
   shader_set_attrib_vertices(shader, "aTexCoord", 2, GL_FLOAT, 0, skybox_tex_coords);
   shader_set_attrib_vertices(shader, "aColor", 3, GL_FLOAT, 0, skybox_colors);

   glCullFace(GL_BACK);
   glDepthFunc(GL_ALWAYS);

   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, skybox_indices);
   glDepthFunc(GL_LESS);
}

void update()
{
   float dt = timers_update();

   cam_update(dt, &camera);

   draw_skybox();

   octree_draw(level, &camera);

   int i = 0;
   for (i = 0; i < nentities; ++i)
   {
      // TODO: implement collision detection
      // physent_apply_force_wcs(&entities[i].phys, &entities[i].phys.position, vec3(0.0f, -9.8f, 0.0f));
      entity_update(&entities[i], dt);
      entity_render(&entities[i], &camera);
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
      cam_slide(&camera, vec3(-coef * dx1, coef * dy1, 0.0f));

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
      cam_slide(&camera, vec3(camera.view_dir.x * v, camera.view_dir.y * v, camera.view_dir.z * v));
   }

   LOGI("Cam pos: [%.2f %.2f %.2f] dir: [%.2f %.2f %.2f]",
        camera.pos.x, camera.pos.y, camera.pos.z,
        camera.view_dir.x, camera.view_dir.y, camera.view_dir.z);
}

void perform_jump()
{
   LOGI("jump");
}

void perform_crouch()
{
   LOGI("crouch");
}

void pointer_down(int pointerId, float x, float y)
{
   LOGI("pointer #%d down: %.2f %.2f", pointerId, x, y);

   if (y >= 0.80f)
   {
      if (x <= 0.15f)
      {
         perform_crouch();
      }
      else if (x >= 0.85f)
      {
         perform_jump();
      }
   }
}

void pointer_up(int pointerId, float x, float y)
{
   LOGI("pointer #%d up: %.2f %.2f", pointerId, x, y);
}

void pointer_move(int pointerId, float dx, float dy)
{
   //LOGI("pointer #%d move: %.2f %.2f", pointerId, dx, dy);
}

