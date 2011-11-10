#include "bbox.h"
#include "common.h"
#include "shader.h"
#include "camera.h"
#include "resman.h"
#include "game.h"

void bbox_reset(bbox_t* b)
{
   b->min.x = b->min.y = b->min.z = 999999.9f;
   b->max.x = b->max.y = b->max.z = -999999.9f;
}

void bbox_inflate(bbox_t* b, vec3f_t* v)
{
   if (b->min.x > v->x) b->min.x = v->x;
   if (b->min.y > v->y) b->min.y = v->y;
   if (b->min.z > v->z) b->min.z = v->z;
   if (b->max.x < v->x) b->max.x = v->x;
   if (b->max.y < v->y) b->max.y = v->y;
   if (b->max.z < v->z) b->max.z = v->z;
}

void bbox_show(const bbox_t* b)
{
   LOGI("BBox: [%.2f %.2f %.2f] [%.2f %.2f %.2f]", b->min.x, b->min.y, b->min.z, b->max.x, b->max.y, b->max.z);
}

static int point_in_bbox(const bbox_t* box, const vec3f_t* p)
{
   if ((box->min.x <= p->x && box->min.y <= p->y && box->min.z <= p->z) &&
         (box->max.x >= p->x && box->max.y >= p->y && box->max.z >= p->z))
   {
      return 1;
   }
   return 0;
}

int bbox_tri_intersection(const bbox_t* bbox, const vec3f_t* a, const vec3f_t* b, const vec3f_t* c)
{
   if (point_in_bbox(bbox, a) != 0) return 1;
   if (point_in_bbox(bbox, b) != 0) return 1;
   if (point_in_bbox(bbox, c) != 0) return 1;
   return 0;
}

int ray_slab_intersection(float start, float dir, float min, float max, float* pfirst, float* plast)
{
   if (fabs(dir) < 1.0E-8)
   {
      return (start < max && start > min) ? 1 : 0;
   }

   float tmin = (min - start) / dir;
   float tmax = (max - start) / dir;

   //LOGI("min %.2f max %.2f start %.2f dir %.2f tmin %.2f tmax %.2f first %.2f last %.2f", min, max, start, dir, tmin, tmax, *pfirst, *plast);

   if (tmin > tmax)
   {
      float t = tmin;
      tmin = tmax;
      tmax = t;
   }

   int res = (tmax < *pfirst || tmin > *plast) ? 0 : 1;
   if (res == 1)
   {
      if (tmin > *pfirst) *pfirst = tmin;
      if (tmax < *plast)  *plast  = tmax;
   }
   return res;
}

int bbox_ray_intersection(const bbox_t* bbox, const vec3f_t* pos, const vec3f_t* dir, float* pt)
{
   float first = 0.0f;
   float last = 99999999.0f;

   if (!ray_slab_intersection(pos->x, dir->x, bbox->min.x, bbox->max.x, &first, &last)) return 0;
   if (!ray_slab_intersection(pos->y, dir->y, bbox->min.y, bbox->max.y, &first, &last)) return 0;
   if (!ray_slab_intersection(pos->z, dir->z, bbox->min.z, bbox->max.z, &first, &last)) return 0;

   *pt = first;
   return 1;
}

extern struct game_t* game;

void bbox_draw(const bbox_t* b, const camera_t* camera)
{
   mat4f_t mvp;
   mat4_mult(&mvp, &camera->proj, &camera->view);

   float vertices[] =
   {
      0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,

      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f,

      0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f,

      0.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f,

      1.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f,
   };

   int i = 0;
   for (i = 0; i < sizeof(vertices)/sizeof(vertices[0]); i += 3)
   {
      vertices[i + 0] = b->min.x * (1.0f - vertices[i + 0]) + b->max.x * vertices[i + 0];
      vertices[i + 1] = b->min.y * (1.0f - vertices[i + 1]) + b->max.y * vertices[i + 1];
      vertices[i + 2] = b->min.z * (1.0f - vertices[i + 2]) + b->max.z * vertices[i + 2];
   }

   shader_t* shader = resman_get_shader(game->resman, "shaders/bbox");
   if (shader == NULL)
      return;

   shader_use(shader);
   shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, 0, &vertices[0]);
   glDrawArrays(GL_LINES, 0, sizeof(vertices)/sizeof(vertices[0])/3);
   shader_unuse(shader);
}

void bbox_transform(bbox_t* b, const mat4f_t* transform)
{
   float vertices[] =
   {
      0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,

      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 1.0f,

      0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 1.0f,

      0.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f,

      1.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f,
   };

   bbox_t bbox;
   bbox_reset(&bbox);
   int i = 0;
   for (i = 0; i < sizeof(vertices)/sizeof(vertices[0]); i += 3)
   {
      vec3f_t vert;
      vert.x = b->min.x * (1.0f - vertices[i + 0]) + b->max.x * vertices[i + 0];
      vert.y = b->min.y * (1.0f - vertices[i + 1]) + b->max.y * vertices[i + 1];
      vert.z = b->min.z * (1.0f - vertices[i + 2]) + b->max.z * vertices[i + 2];

      vec3f_t transformed;
      mat4_mult_vec3(&transformed, transform, &vert);
      bbox_inflate(&bbox, &transformed);
   }

   *b = bbox;
}

