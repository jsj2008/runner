#include "hlmdl.h"
#include "common.h"
#include "mesh.h"
#include "assets.h"
#include <stdio.h>
#include <string.h>

typedef float vec3_t[3];
typedef float vec4_t[3];

typedef struct mdl_header_t
{
   long id;
   long version;
   char name[64];
   long length;
   vec3_t eye_position;
   vec3_t min;
   vec3_t max;
   vec3_t bbox_min;
   vec3_t bbox_max;
   long flags;
   long nbones;
   long bone_offset;
   long nbonectrls;
   long bonectrl_offset;
   long nhitboxes;
   long hitbox_offset;
   long nsequences;
   long sequence_offset;
   long nsequencegroups;
   long sequencegroup_offset;
   long ntextures;
   long texture_offset;
   long texture_data;
   long nskinrefs;
   long nskinfamilies;
   long skin_offset;
   long nbodyparts;
   long bodypart_offset;
   long nattaches;
   long attach_offset;
   long snd_table;
   long snd_offset;
   long nsndgroups;
   long sndgroup_offset;
   long ntransitions;
   long transition_offset;
} mdl_header_t;

typedef struct mdl_seq_header_t
{
   long id;
   long version;
   char name[64];
   long length;
} mdl_seq_header_t;

typedef struct mdl_bone_t
{
   char name[32];
   long parent;
   long flags;
   long bone_ctrl[6];
   float value[6];
   float scale[6];
} mdl_bone_t;

typedef struct mdl_texture_t
{
   char name[64];
   long flags;
   long width;
   long height;
   long offset;
} mdl_texture_t;

typedef struct mdl_bodypart_t
{
   char name[64];
   long nmodels;
   long base;
   long model_offset;
} mdl_bodypart_t;

typedef struct mdl_model_t
{
   char name[64];
   long type;
   float bounding_radius;
   long nmesh;
   long mesh_offset;
   long nvertices;
   long vertex_info_offset;
   long vertex_offset;
   long nnormals;
   long normal_info_offset;
   long normal_offset;
   long ngroups;
   long group_offset;
} mdl_model_t;

typedef struct mdl_mesh_t
{
   long ntriangles;
   long triangle_offset;
   long skin_ref;
   long nnormals;
   long normal_offset;
} mdl_mesh_t;

void print_header_info(const mdl_header_t* h)
{
   LOGI("ID: 0x%08lX Version: %ld Length: %ld\n", h->id, h->version, h->length);
   LOGI("Model name:\t%s\n", h->name);
   LOGI("Box:\t[%.2f %.2f %.2f] [%.2f %.2f %.2f]\n", h->min[0], h->min[1], h->min[2], h->max[0], h->max[1], h->max[2]);
   LOGI("Bbox:\t[%.2f %.2f %.2f] [%.2f %.2f %.2f]\n", h->bbox_min[0], h->bbox_min[1], h->bbox_min[2], h->bbox_max[0], h->bbox_max[1], h->bbox_max[2]);
   LOGI("Bones:\t\t%ld\toffset:0x%08lX\n", h->nbones, h->bone_offset);
   LOGI("Controllers:\t%ld\toffset:0x%08lX\n", h->nbonectrls, h->bonectrl_offset);
   LOGI("Hitboxes:\t%ld\toffset:0x%08lX\n", h->nhitboxes, h->hitbox_offset);
   LOGI("Sequences:\t%ld\toffset:0x%08lX\n", h->nsequences, h->sequence_offset);
   LOGI("Seq groups:\t%ld\toffset:0x%08lX\n", h->nsequencegroups, h->sequencegroup_offset);
   LOGI("Textures:\t%ld\toffset:0x%08lX\n", h->ntextures, h->texture_offset);
   LOGI("Skin refs:\t%ld\toffset:0x%08lX\n", h->nskinrefs, h->skin_offset);
   LOGI("Skin families:\t%ld\toffset:0x%08lX\n", h->nskinfamilies, h->skin_offset);
   LOGI("Bodyparts:\t%ld\toffset:0x%08lX\n", h->nbodyparts, h->bodypart_offset);
   LOGI("Attachments:\t%ld\toffset:0x%08lX\n", h->nattaches, h->attach_offset);
   LOGI("Sound groups:\t%ld\toffset:0x%08lX\n", h->nsndgroups, h->sndgroup_offset);
   LOGI("Transitions:\t%ld\toffset:0x%08lX\n", h->ntransitions, h->transition_offset);

   long i = 0;
   long j = 0;
   long k = 0;
   /*for (i = 0; i < h->nsequences; ++i)
   {
      const mdl_seq_header_t* s = (const mdl_seq_header_t*)(((const char*)h) + h->sequence_offset + i*sizeof(mdl_seq_header_t));

      LOGI("Sequence #%04ld: %s length:%ld\t\n", i, s->name, s->length);
   }*/
   for (i = 0; i < h->nbones; ++i)
   {
      const mdl_bone_t* s = (const mdl_bone_t*)(((const char*)h) + h->bone_offset + i*sizeof(mdl_bone_t));
      LOGI("Bone #%04ld: %s parent: #%04ld\n", i, s->name, s->parent);
   }
   for (i = 0; i < h->nbodyparts; ++i)
   {
      const mdl_bodypart_t* s = (const mdl_bodypart_t*)(((const char*)h) + h->bodypart_offset + i*sizeof(mdl_bodypart_t));
      LOGI("Bodypart #%04ld: %s nmodels: %ld base: 0x%08lX\n", i, s->name, s->nmodels, s->base);
      for (j = 0; j < s->nmodels; ++j)
      {
         const mdl_model_t* m = (const mdl_model_t*)(((const char*)h) + s->model_offset + j*sizeof(mdl_model_t));
         LOGI("\tModel #%04ld: %s nmesh: %ld nverts: %ld nnorms: %ld\n", j, m->name, m->nmesh, m->nvertices, m->nnormals);

         const vec3_t* verts = (const vec3_t*)(((const char*)h) + m->vertex_offset);
         for (k = 0; k < m->nvertices; ++k)
         {
            LOGI("\t\tVertex: %.2f %.2f %.2f\n", verts[k][0], verts[k][1], verts[k][2]);
         }

         for (k = 0; k < m->nmesh; ++k)
         {
            const mdl_mesh_t* q = (const mdl_mesh_t*)(((const char*)h) + m->mesh_offset + k*sizeof(mdl_mesh_t));
            LOGI("\t\tMesh #%04ld: skin: %ld ntris: %ld offset: %08lX nnormals: %ld offset: 0x%08lX\n", k, q->skin_ref, q->ntriangles, q->triangle_offset, q->nnormals, q->normal_offset);
         }
      }
   }
   for (i = 0; i < h->ntextures; ++i)
   {
      const mdl_texture_t* s = (const mdl_texture_t*)(((const char*)h) + h->texture_offset + i*sizeof(mdl_texture_t));
      LOGI("Texture #%04ld: %s [%ldx%ld] flags: %ld offset: 0x%08lX\n", i, s->name, s->width, s->height, s->flags, s->offset);
   }
}

#define MAX_BONES 256
static mat4f_t bone_transforms[MAX_BONES];

long add_vertex(const short* t, vertex_t* buf, long* nverts, const vec3_t* v, const vec3_t* n, const unsigned char* vert_info, const mdl_bone_t* bones)
{
   short vi = t[0];
   short ni = t[1];
   short tu = t[2];
   short tv = t[3];

   unsigned char bi = vert_info[vi];

   vertex_t vert;
   vert.normal.x = n[ni][0];
   vert.normal.y = n[ni][1];
   vert.normal.z = n[ni][2];
   vert.normal.w = 0.0f;
   vert.tex_coord[0] = 256/(float)tu;
   vert.tex_coord[1] = 256/(float)tv;

   vec4f_t pos;
   pos.x = v[vi][0];
   pos.y = v[vi][1];
   pos.z = v[vi][2];
   pos.w = 0.0f;
   mat4_mult_vector(&vert.pos, &bone_transforms[bi], &pos);

   LOGI("Vector %.2f %.2f %.2f %.2f transformed to %.2f %.2f %.2f %.2f using bone #%04d", pos.x, pos.y, pos.z, pos.w, vert.pos.x, vert.pos.y, vert.pos.z, vert.pos.w, bi);

   long index = -1;
   long i = 0;
   for (i = 0; i < (*nverts); ++i)
   {
      if (memcmp(&buf[i], &vert, sizeof(vertex_t)) == 0)
      {
         //LOGI("Found vertex #%04ld\n", i);
         index = i;
         break;
      }
   }

   if (index == -1)
   {
      //LOGI("Adding vertex #%04ld\n", *nverts);
      index = (*nverts)++;
      buf[index] = vert;
   }

   return index;
}

int mesh_load_from_mdl(const char* fname, mesh_t* m)
{
   LOGI("Loading model from %s\n", fname);

   int size = 0;
   char* buf = readFile(fname, &size);
   if (buf == NULL)
   {
      LOGE("Unable to read model from %s\n", fname);
      return -1;
   }

   LOGI("read %d bytes from %s\n", size, fname);

   mdl_header_t* header = (mdl_header_t*)buf;
   //print_header_info(header);

   if (header->id != *(long*)"IDST")
   {
      LOGE("Invalid file signature\n");
      return -1;
   }

   if (header->length != size)
   {
      LOGE("Broken file [file length=%ld header specified length=%ld]\n", size, header->length);
      return -1;
   }

   long j = 0;
   long k = 0;

   if (header->nbodyparts > 0)
   {
      // use first bodypart
      const mdl_bodypart_t* part = (const mdl_bodypart_t*)(buf + header->bodypart_offset);
      if (part->nmodels > 0)
      {
         // use first model
         const mdl_model_t* mdl = (const mdl_model_t*)(buf + part->model_offset);
         const vec3_t* v = (const vec3_t*)(buf + mdl->vertex_offset);
         const vec3_t* n = (const vec3_t*)(buf + mdl->normal_offset);

         const unsigned char* vi = (const unsigned char*)(buf + mdl->vertex_info_offset);
         const mdl_bone_t* bones = (const mdl_bone_t*)(buf + header->bone_offset);


         // calculate bones transforms
         for (j = 0; j < header->nbones; ++j)
         {
            LOGI("Bone #%04ld: %s parent: #%04ld pos: %.2f %.2f %.2f angles: %.2f %.2f %.2f scale: [%.2f %.2f %.2f] [%.2f %.2f %.2f]\n", j, bones->name, bones->parent, bones->value[0], bones->value[1], bones->value[2], bones->value[3], bones->value[4], bones->value[5], bones->scale[0], bones->scale[1], bones->scale[2], bones->scale[3], bones->scale[4], bones->scale[5]);

            vec4f_t pos;
            pos.x = bones->value[0];
            pos.y = bones->value[1];
            pos.z = bones->value[2];
            pos.w = 1.0f; 

            vec4f_t angles;
            angles.x = bones->value[3];
            angles.y = bones->value[4];
            angles.z = bones->value[5];

            vec4f_t quat;
            quat_from_angles(&quat, &angles);

            LOGI("Quaternion: %.2f %.2f %.2f %.2f", quat.x, quat.y, quat.z, quat.w);

            mat4f_t transform;
            mat4_from_quaternion(&transform, &quat);
            transform.m41 = pos.x;
            transform.m42 = pos.y;
            transform.m43 = pos.z;

            if (bones->parent == -1)
            {
               bone_transforms[j] = transform;
            }
            else
            {
               mat4_mult(&bone_transforms[j], &bone_transforms[bones->parent], &transform);
            }

            mat4_show(&bone_transforms[j]);
            ++bones;
         }

         vertex_t verts_buf[8192];
         int indices_buf[16386];
         long vert_index = 0;
         long ind_index = 0;

         memset(verts_buf, 0, sizeof(verts_buf));
         memset(indices_buf, 0, sizeof(indices_buf));
         for (j = 0; j < mdl->nmesh; ++j)
         {
            const mdl_mesh_t* mm = (const mdl_mesh_t*)(buf + mdl->mesh_offset) + j;
            const short* t = (const short*)(buf + mm->triangle_offset);
            long ntris = 0;
            while ((ntris = *(t++)) != 0)
            {
               //LOGI("NTRIS: %ld\n", ntris);
               if (ntris < 0)
               {
                  // GL_TRIANGLE_FAN
                  ntris = -ntris;

                  long first = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                  t += 4;
                  long prev = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                  t += 4;

                  for (k = 2; k < ntris; ++k, t += 4)
                  {
                     long current = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                     indices_buf[ind_index++] = first;
                     indices_buf[ind_index++] = prev;
                     indices_buf[ind_index++] = current;
                     //LOGI("TRIANGLE: %04ld %04ld %04ld\n", first, prev, current);
                     prev = current;
                  }
               }
               else
               {
                  // GL_TRIANGLE_STRIP
                  long n0 = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                  t += 4;
                  long n1 = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                  t += 4;

                  int odd = 1;
                  for (k = 2; k < ntris; ++k, t += 4)
                  {
                     long n2 = add_vertex(t, verts_buf, &vert_index, v, n, vi, bones);
                     if (odd == 1)
                     {
                        indices_buf[ind_index++] = n0;
                        indices_buf[ind_index++] = n1;
                        indices_buf[ind_index++] = n2;
                        //LOGI("TRIANGLE: %04ld %04ld %04ld\n", n0, n1, n2);
                        odd = 0;
                     }
                     else
                     {
                        indices_buf[ind_index++] = n1;
                        indices_buf[ind_index++] = n0;
                        indices_buf[ind_index++] = n2;
                        //LOGI("TRIANGLE: %04ld %04ld %04ld\n", n1, n0, n2);
                        odd = 1;
                     }

                     n0 = n1;
                     n1 = n2;
                  }
               }
            }
         }

         m->nvertices = vert_index;
         m->vertices = (vertex_t*)malloc(m->nvertices*sizeof(vertex_t));
         memcpy(m->vertices, verts_buf, m->nvertices*sizeof(vertex_t));

         m->nindices = ind_index;
         m->indices = (int*)malloc(m->nindices*sizeof(int));
         memcpy(m->indices, indices_buf, m->nindices*sizeof(int));

         LOGI("MESH VERTICES: %d INDICES: %d\n", m->nvertices, m->nindices);
      }
   }

   free(buf);

   return 0;
}

