#include "hlmdl.h"
#include "common.h"
#include "model.h"
#include "assets.h"

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

typedef struct mdl_seq_desc_t
{
   char name[32];
   float fps;
   int flags;
   int activity;
   int actweight;
   int nevents;
   int event_offset;
   int nframes;
   int npivots;
   int pivot_offset;
   int motion_type;
   int motion_bone;
   vec3_t linear_movement;
   int automoveposindex;
   int automoveangleindex;
   vec3_t bbox_min;
   vec3_t bbox_max;
   int nblends;
   int anim_offset;
   int blend_type[2];
   float blend_start[2];
   float blend_end[2];
   int blend_parent;
   int seq_group;
   int entry_node;
   int exit_node;
   int node_flags;
   int seq_next;
} mdl_seq_desc_t;

typedef struct mdl_seq_group_t
{
   char name[32];
   char fname[64];
   void* cache;
   int data;
} mdl_seq_group_t;

typedef struct mdl_anim_t
{
   unsigned short offset[6];
} mdl_anim_t;

typedef union mdl_animvalue_t
{
   struct
   {
      unsigned char valid;
      unsigned char total;
   } num;
   short value;
} mdl_animvalue_t;

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

mdl_header_t* g_texturehdr = NULL;
mdl_header_t* g_animhdr[32];

mdl_anim_t* get_anim(const mdl_header_t* h, const mdl_seq_desc_t* seq)
{
   mdl_seq_group_t* g = (mdl_seq_group_t*)((char*)h + h->sequencegroup_offset) + seq->seq_group;
   if (seq->seq_group == 0)
   {
      return (mdl_anim_t*)((char*)h + g->data + seq->anim_offset);
   }
   else
   {
      return (mdl_anim_t*)((char*)g_animhdr[seq->seq_group] + seq->anim_offset);
   }
}

void print_header_info(const mdl_header_t* h)
{
   LOGI("ID: 0x%08lX Version: %ld Length: %ld", h->id, h->version, h->length);
   LOGI("Model name:\t%s", h->name);
   LOGI("Box:\t[%.2f %.2f %.2f] [%.2f %.2f %.2f]", h->min[0], h->min[1], h->min[2], h->max[0], h->max[1], h->max[2]);
   LOGI("Bbox:\t[%.2f %.2f %.2f] [%.2f %.2f %.2f]", h->bbox_min[0], h->bbox_min[1], h->bbox_min[2], h->bbox_max[0], h->bbox_max[1], h->bbox_max[2]);
   LOGI("Bones:\t\t%ld\toffset:0x%08lX", h->nbones, h->bone_offset);
   LOGI("Controllers:\t%ld\toffset:0x%08lX", h->nbonectrls, h->bonectrl_offset);
   LOGI("Hitboxes:\t%ld\toffset:0x%08lX", h->nhitboxes, h->hitbox_offset);
   LOGI("Sequences:\t%ld\toffset:0x%08lX", h->nsequences, h->sequence_offset);
   LOGI("Seq groups:\t%ld\toffset:0x%08lX", h->nsequencegroups, h->sequencegroup_offset);
   LOGI("Textures:\t%ld\toffset:0x%08lX", h->ntextures, h->texture_offset);
   LOGI("Skin refs:\t%ld\toffset:0x%08lX", h->nskinrefs, h->skin_offset);
   LOGI("Skin families:\t%ld\toffset:0x%08lX", h->nskinfamilies, h->skin_offset);
   LOGI("Bodyparts:\t%ld\toffset:0x%08lX", h->nbodyparts, h->bodypart_offset);
   LOGI("Attachments:\t%ld\toffset:0x%08lX", h->nattaches, h->attach_offset);
   LOGI("Sound groups:\t%ld\toffset:0x%08lX", h->nsndgroups, h->sndgroup_offset);
   LOGI("Transitions:\t%ld\toffset:0x%08lX", h->ntransitions, h->transition_offset);

   long i = 0;
   long j = 0;
   long k = 0;

   for (i = 0; i < h->nbones; ++i)
   {
      const mdl_bone_t* s = (const mdl_bone_t*)(((const char*)h) + h->bone_offset + i*sizeof(mdl_bone_t));
      LOGI("Bone #%04ld: %s parent: #%04ld", i, s->name, s->parent);
   }

   for (i = 0; i < h->nbodyparts; ++i)
   {
      const mdl_bodypart_t* s = (const mdl_bodypart_t*)(((const char*)h) + h->bodypart_offset + i*sizeof(mdl_bodypart_t));
      LOGI("Bodypart #%04ld: %s nmodels: %ld base: 0x%08lX", i, s->name, s->nmodels, s->base);
      for (j = 0; j < s->nmodels; ++j)
      {
         const mdl_model_t* m = (const mdl_model_t*)(((const char*)h) + s->model_offset + j*sizeof(mdl_model_t));
         LOGI("\tModel #%04ld: %s nmesh: %ld nverts: %ld nnorms: %ld", j, m->name, m->nmesh, m->nvertices, m->nnormals);

         /*const vec3_t* verts = (const vec3_t*)(((const char*)h) + m->vertex_offset);
         for (k = 0; k < m->nvertices; ++k)
         {
            LOGI("\t\tVertex: %.2f %.2f %.2f", verts[k][0], verts[k][1], verts[k][2]);
         }*/

         for (k = 0; k < m->nmesh; ++k)
         {
            const mdl_mesh_t* q = (const mdl_mesh_t*)(((const char*)h) + m->mesh_offset + k*sizeof(mdl_mesh_t));
            LOGI("\t\tMesh #%04ld: skin: %ld ntris: %ld offset: %08lX nnormals: %ld offset: 0x%08lX", k, q->skin_ref, q->ntriangles, q->triangle_offset, q->nnormals, q->normal_offset);
         }
      }
   }
   for (i = 0; i < h->ntextures; ++i)
   {
      const mdl_texture_t* s = (const mdl_texture_t*)(((const char*)h) + h->texture_offset + i*sizeof(mdl_texture_t));
      LOGI("Texture #%04ld: %s [%ldx%ld] flags: %ld offset: 0x%08lX", i, s->name, s->width, s->height, s->flags, s->offset);
   }
}

#define MAX_BONES 256

static mat4f_t bone_transforms[MAX_BONES];

long add_vertex(const short* t, vertex_t* buf, long* nverts, const vec3_t* v, const vec3_t* n, const unsigned char* vert_info)
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
   vert.bone[0] = bi;
   vert.bone[1] = 0;
   vert.weight[0] = 1.0f;
   vert.weight[1] = 0.0f;

   vec4f_t pos;
   pos.x = v[vi][0];
   pos.y = v[vi][1];
   pos.z = v[vi][2];
   pos.w = 0.0f;
   mat4_mult_vector(&vert.pos, &bone_transforms[bi], &pos);

   //LOGI("Vector %.2f %.2f %.2f %.2f transformed to %.2f %.2f %.2f %.2f using bone #%04d", pos.x, pos.y, pos.z, pos.w, vert.pos.x, vert.pos.y, vert.pos.z, vert.pos.w, bi);

   long index = -1;
   long i = 0;
   for (i = 0; i < (*nverts); ++i)
   {
      if (memcmp(&buf[i], &vert, sizeof(vertex_t)) == 0)
      {
         index = i;
         break;
      }
   }

   if (index == -1)
   {
      index = (*nverts)++;
      buf[index] = vert;
   }

   return index;
}

void get_anim_values(const mdl_anim_t* anim, int frame, short values[6])
{
   int i = 0;
   for (i = 0; i < 6; ++i)
   {
      if (anim->offset[i] != 0)
      {
         mdl_animvalue_t* av = (mdl_animvalue_t*)((char*)anim + anim->offset[i]);

         int k = frame;

         while (av->num.total <= k)
         {
            k -= av->num.total;
            av += av->num.valid + 1;
         }
         //LOGI("#%d[%d]: AnimValue: %d [%d %d]", i, k, av->value, (int)av->num.valid, (int)av->num.total);
         if (av->num.valid > k)
         {
            values[i] = av[k + 1].value;
         }
         else
         {
            values[i] = av[av->num.valid].value;
         }
      }
      else
      {
         values[i] = 0;
      }
   }
}

mdl_header_t* read_hlmdl(const char* fname)
{
   LOGI("Loading hl model from %s", fname);

   long size = 0;
   char* buf = readFile(fname, &size);
   if (buf == NULL)
   {
      LOGE("Unable to read model from %s", fname);
      return NULL;
   }

   mdl_header_t* header = (mdl_header_t*)buf;

   if (  (header->id != *(long*)"IDST") &&
         (header->id != *(long*)"IDSQ"))
   {
      LOGE("Invalid file signature");
      return NULL;
   }

   if (header->length != size)
   {
      LOGE("Broken file [file length=%ld header specified length=%ld]", size, header->length);
      return NULL;
   }

   return header;
}

int hlmdl_convert(const char* from, const char* to)
{
   mdl_header_t* header = read_hlmdl(from);
   if (header == NULL)
   {
      return -1;
   }

   print_header_info(header);

   long i = 0;
   long j = 0;
   long k = 0;
   long l = 0;

   if (header->ntextures == 0)
   {
      char name[256] = {0};

      strncpy(name, from, strlen(from) - 4);
      strcat(name, "T.mdl");

      g_texturehdr = read_hlmdl(name);
      if (g_texturehdr == NULL)
      {
         return -1;
      }
      print_header_info(g_texturehdr);
   }

   if (header->nsequencegroups > 1)
   {
      for (i = 1; i < header->nsequencegroups; ++i)
      {
         char name[256] = {0};
         char index[32] = {0};

         strncpy(name, from, strlen(from) - 4);
         sprintf(index, "%02ld.mdl", i);
         strcat(name, index);

         g_animhdr[i] = read_hlmdl(name);
         if (g_animhdr[i] == NULL)
         {
            return -1;
         }
      }
   }

   model_t model =
   {
      .nmeshes = header->nbodyparts,
      .nanims = header->nsequences,
      .nbones = header->nbones,
      .bones = (bone_t*)malloc(header->nbones * sizeof(bone_t)),
      .anims = (anim_t*)malloc(header->nsequences * sizeof(anim_t)),
      .meshes = (mesh_t*)malloc(header->nbodyparts * sizeof(mesh_t)),
   };

   strcpy(model.name, header->name);

   bone_t* bones = &model.bones[0];
   anim_t* anims = &model.anims[0];
   mesh_t* mesh = &model.meshes[0];

   const mdl_bone_t* bone = (const mdl_bone_t*)((char*)header + header->bone_offset);
   for (l = 0; l < header->nbones; ++l)
   {
      strcpy(bones[l].name, bone->name);
      bones[l].parent = bone->parent;

      bones[l].pos.x = bone->value[0];
      bones[l].pos.y = bone->value[0];
      bones[l].pos.z = bone->value[0];
      bones[l].pos.w = 1.0f;

      vec4f_t angles;
      angles.x = bone->value[3];
      angles.y = bone->value[4];
      angles.z = bone->value[5];

      quat_from_angles(&bones[l].quat, &angles);

      ++bone;
   }

   mdl_seq_desc_t* seq = (mdl_seq_desc_t*)((char*)header + header->sequence_offset);
   for (j = 0; j < header->nsequences; ++j)
   {
      anim_t* a = &anims[j];
      strcpy(a->name, seq->name);
      a->nframes = seq->nframes;
      a->transforms = (float*)malloc(seq->nframes * header->nbones * 6 * sizeof(float));

      //LOGI("Processing sequence #%ld: %s [%d frames] [transforms %ld bytes]", j, a->name, a->nframes, seq->nframes * header->nbones * 6 * sizeof(float));

      float* transform = &a->transforms[0];
      for (k = 0; k < seq->nframes; ++k)
      {
         mdl_anim_t* anim = get_anim(header, seq);
         const mdl_bone_t* bone = (const mdl_bone_t*)((char*)header + header->bone_offset);
         for (l = 0; l < header->nbones; ++l)
         {
            //LOGI("Bone #%04ld: %s parent: #%04ld pos: %.2f %.2f %.2f angles: %.2f %.2f %.2f scale: [%.2f %.2f %.2f] [%.2f %.2f %.2f]", j, bone->name, bone->parent, bone->value[0], bone->value[1], bone->value[2], bone->value[3], bone->value[4], bone->value[5], bone->scale[0], bone->scale[1], bone->scale[2], bone->scale[3], bone->scale[4], bone->scale[5]);

            short values[6] = {0};
            get_anim_values(anim, k, values);

            vec4f_t quat0;
            vec4f_t angles0;
            angles0.x = bone->value[3];
            angles0.y = bone->value[4];
            angles0.z = bone->value[5];
            quat_from_angles(&quat0, &angles0);

            vec4f_t quat1;
            vec4f_t angles1;
            angles1.x = angles0.x + values[3] * bone->scale[3];
            angles1.y = angles0.y + values[4] * bone->scale[4];
            angles1.z = angles0.z + values[5] * bone->scale[5];
            quat_from_angles(&quat1, &angles1);

            vec4f_t qdiff;
            vec4f_t iquat0;
            iquat0.x = -quat0.x;
            iquat0.y = -quat0.y;
            iquat0.z = -quat0.z;
            iquat0.w = quat0.w;
            quat_mult(&qdiff, &quat1, &iquat0);

            transform[0] = values[0] * bone->scale[0];
            transform[1] = values[1] * bone->scale[1];
            transform[2] = values[2] * bone->scale[2];
            transform[3] = qdiff.x;
            transform[4] = qdiff.y;
            transform[5] = qdiff.z;
            transform += 6;

            ++bone;
            ++anim;
         }
      }

      ++seq;
   }

   const mdl_bodypart_t* part = (const mdl_bodypart_t*)((char*)header + header->bodypart_offset);
   for (i = 0; i < header->nbodyparts; ++i)
   {
      if (part->nmodels > 0)
      {
         // use first model
         const mdl_model_t* mdl = (const mdl_model_t*)((char*)header + part->model_offset);
         const vec3_t* v = (const vec3_t*)((char*)header + mdl->vertex_offset);
         const vec3_t* n = (const vec3_t*)((char*)header + mdl->normal_offset);

         const unsigned char* vi = (const unsigned char*)((char*)header + mdl->vertex_info_offset);

         /*
         // calculate bones transforms
         const mdl_bone_t* bone = (const mdl_bone_t*)((char*)header + header->bone_offset);
         const mdl_anim_t* anim = get_anim(header, seq);
         for (j = 0; j < header->nbones; ++j)
         {
            short values[6] = {0};
            get_anim_values(anim++, 0, values);

            vec4f_t pos;
            pos.x = bone->value[0] + values[0] * bone->scale[0];
            pos.y = bone->value[1] + values[1] * bone->scale[2];
            pos.z = bone->value[2] + values[2] * bone->scale[2];
            pos.w = 1.0f;

            vec4f_t angles;
            angles.x = bone->value[3] + values[3] * bone->scale[3];
            angles.y = bone->value[4] + values[4] * bone->scale[4];
            angles.z = bone->value[5] + values[5] * bone->scale[5];

            vec4f_t quat;
            quat_from_angles(&quat, &angles);
            //LOGI(" A: %.2f %.2f %.2f", angles.x, angles.y, angles.z);
            //LOGI("QA: %.2f %.2f %.2f %.2f", quat.x, quat.y, quat.z, quat.w);

            mat4f_t transform;
            mat4_from_quaternion(&transform, &quat);
            transform.m14 = pos.x;
            transform.m24 = pos.y;
            transform.m34 = pos.z;
            transform.m44 = 1.0f;

            if (bone->parent == -1)
            {
               bone_transforms[j] = transform;
            }
            else
            {
               mat4_mult(&bone_transforms[j], &bone_transforms[bone->parent], &transform);
            }
            ++bone;
         }*/

         vertex_t verts_buf[8192];
         int indices_buf[16386];
         long vert_index = 0;
         long ind_index = 0;

         memset(verts_buf, 0, sizeof(verts_buf));
         memset(indices_buf, 0, sizeof(indices_buf));
         for (j = 0; j < mdl->nmesh; ++j)
         {
            const mdl_mesh_t* mm = (const mdl_mesh_t*)((char*)header + mdl->mesh_offset) + j;
            const short* t = (const short*)((char*)header + mm->triangle_offset);
            long ntris = 0;
            while ((ntris = *(t++)) != 0)
            {
               //LOGI("NTRIS: %ld", ntris);
               if (ntris < 0)
               {
                  // GL_TRIANGLE_FAN
                  ntris = -ntris;

                  long first = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                  t += 4;
                  long prev = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                  t += 4;

                  for (k = 2; k < ntris; ++k, t += 4)
                  {
                     long current = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                     indices_buf[ind_index++] = first;
                     indices_buf[ind_index++] = prev;
                     indices_buf[ind_index++] = current;
                     //LOGI("TRIANGLE: %04ld %04ld %04ld", first, prev, current);
                     prev = current;
                  }
               }
               else
               {
                  // GL_TRIANGLE_STRIP
                  long n0 = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                  t += 4;
                  long n1 = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                  t += 4;

                  int odd = 1;
                  for (k = 2; k < ntris; ++k, t += 4)
                  {
                     long n2 = add_vertex(t, verts_buf, &vert_index, v, n, vi);
                     if (odd == 1)
                     {
                        indices_buf[ind_index++] = n0;
                        indices_buf[ind_index++] = n1;
                        indices_buf[ind_index++] = n2;
                        //LOGI("TRIANGLE: %04ld %04ld %04ld", n0, n1, n2);
                        odd = 0;
                     }
                     else
                     {
                        indices_buf[ind_index++] = n1;
                        indices_buf[ind_index++] = n0;
                        indices_buf[ind_index++] = n2;
                        //LOGI("TRIANGLE: %04ld %04ld %04ld", n1, n0, n2);
                        odd = 1;
                     }

                     n0 = n1;
                     n1 = n2;
                  }
               }
            }
         }

         mesh->nvertices = vert_index;
         mesh->vertices = (vertex_t*)malloc(mesh->nvertices*sizeof(vertex_t));
         memcpy(mesh->vertices, verts_buf, mesh->nvertices*sizeof(vertex_t));

         mesh->nindices = ind_index;
         mesh->indices = (int*)malloc(mesh->nindices*sizeof(int));
         memcpy(mesh->indices, indices_buf, mesh->nindices*sizeof(int));
      }

      strcpy(mesh->name, part->name);
      ++mesh;
      ++part;
   }
   model_save(&model, to);

   free(header);

   return 0;
}

