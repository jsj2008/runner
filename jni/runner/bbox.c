#include "bbox.h"
#include "common.h"
#include "shader.h"

void bbox_reset(bbox_t* b)
{
   b->min.x = b->min.y = b->min.z = 999999.9f;
   b->max.x = b->max.y = b->max.z = -999999.9f;
}

void bbox_inflate(bbox_t* b, vec4f_t* v)
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

static int point_in_bbox(const bbox_t* box, const vec4f_t* p)
{
   if ((box->min.x <= p->x && box->min.y <= p->y && box->min.z <= p->z) &&
         (box->max.x >= p->x && box->max.y >= p->y && box->max.z >= p->z))
   {
      return 1;
   }
   return 0;
}

int bbox_tri_intersection(const bbox_t* bbox, const vec4f_t* a, const vec4f_t* b, const vec4f_t* c)
{
   if (point_in_bbox(bbox, a) != 0) return 1;
   if (point_in_bbox(bbox, b) != 0) return 1;
   if (point_in_bbox(bbox, c) != 0) return 1;
   return 0;
}

extern shader_t bbox_shader;

void bbox_draw(const bbox_t* b, const cam_t* camera)
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
   vec4f_t v;
   for (i = 0; i < sizeof(vertices)/sizeof(vertices[0]); i += 3)
   {
      vertices[i + 0] = b->min.x * (1.0f - vertices[i + 0]) + b->max.x * vertices[i + 0];
      vertices[i + 1] = b->min.y * (1.0f - vertices[i + 1]) + b->max.y * vertices[i + 1];
      vertices[i + 2] = b->min.z * (1.0f - vertices[i + 2]) + b->max.z * vertices[i + 2];
   }

   shader_use(&bbox_shader);
   shader_set_uniform_matrices(&bbox_shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(&bbox_shader, "aPos", 3, GL_FLOAT, 0, &vertices[0]);
   glDrawArrays(GL_LINES, 0, sizeof(vertices)/sizeof(vertices[0])/3);
}

