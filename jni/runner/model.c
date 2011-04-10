#include "model.h"
#include "common.h"
#include "gl.h"

typedef struct model_header_t
{
   char magic[8];
   long version;
   long data_offset;
   long data_size;
} model_header_t;

#define WRITE(n)     { header.data_size += fwrite(&n, 1, sizeof(n), f); }
#define WRITEA(n, c) { header.data_size += fwrite(&n[0], 1, c * sizeof(n[0]), f); }

int model_save(const model_t* model, const char* fname)
{
   LOGI("Saving model %s to %s", model->name, fname);

   FILE* f = fopen(fname, "wb");
   if (f == NULL)
   {
      LOGE("Unable to create file: %s", fname);
      return -1;
   }

   model_header_t header =
   {
      .magic = "RNNRMDL_",
      .version = 1,
      .data_offset = sizeof(model_header_t),
      .data_size = 0,
   };

   long i = 0;

   // write model info
   fseek(f, header.data_offset, SEEK_SET);

   model_t mdl = (*model);
   mdl.bones = (bone_t*)(sizeof(model_t));
   mdl.anims = (anim_t*)((char*)mdl.bones + mdl.nbones * sizeof(bone_t));
   mdl.meshes = (mesh_t*)((char*)mdl.anims + mdl.nanims * sizeof(anim_t));

   WRITE(mdl);
   WRITEA(model->bones, model->nbones);

   long offset = (long)((char*)mdl.meshes + mdl.nmeshes * sizeof(mesh_t));
   anim_t* anim = &model->anims[0];
   for (i = 0; i < mdl.nanims; ++i)
   {
      anim_t a = (*anim);
      a.transforms = (float*)offset;
      offset += anim->nframes * model->nbones * 6 * sizeof(float);

      WRITE(a);

      ++anim;
   }

   mesh_t* mesh = &model->meshes[0];
   for (i = 0; i < mdl.nmeshes; ++i)
   {
      mesh_t m = (*mesh);

      m.vertices = (vertex_t*)offset;
      offset += m.nvertices * sizeof(vertex_t);

      m.indices = (int*)offset;
      offset += m.nindices * sizeof(int);

      WRITE(m);

      ++mesh;
   }

   anim = &model->anims[0];
   for (i = 0; i < model->nanims; ++i)
   {
      WRITEA(anim->transforms, anim->nframes * model->nbones * 6);
      ++anim;
   }

   mesh = &model->meshes[0];
   for (i = 0; i < model->nmeshes; ++i)
   {
      WRITEA(mesh->vertices, mesh->nvertices);
      WRITEA(mesh->indices, mesh->nindices);
      ++mesh;
   }

   // write header
   fseek(f, 0, SEEK_SET);
   WRITE(header);

   fclose(f);
   return 0;
}

int model_load(model_t** m, const char* fname)
{
   LOGI("Loading model from %s", fname);

   if (m == NULL || fname == NULL)
   {
      LOGE("Invalid arguments");
      return -1;
   }

   FILE* f = fopen(fname, "rb");
   if (f == NULL)
   {
      LOGE("Unable to open file: %s", fname);
      return -1;
   }

   // read header
   model_header_t header;
   fread(&header, 1, sizeof(model_header_t), f);
   if (memcmp(header.magic, "RNNRMDL_", sizeof(header.magic)) != 0)
   {
      LOGE("Invalid file signature: %s", header.magic);
      fclose(f);
      return -1;
   }

   fseek(f, 0, SEEK_END);
   long size = ftell(f);

   if (header.data_offset + header.data_size > size)
   {
      LOGE("Invalid file size [offset: %ld size: %ld filesize: %ld]", header.data_offset, header.data_size, size);
      fclose(f);
      return -1;
   }

   LOGI("Header: offset=%ld size=%ld", header.data_offset, header.data_size);

   fseek(f, header.data_offset, SEEK_SET);
   char* buf = malloc(header.data_size);
   fread(buf, 1, header.data_size, f);
   fclose(f);

   model_t* mdl = (model_t*)buf;
   mdl->bones = (bone_t*)(buf + (long)mdl->bones);
   mdl->anims = (anim_t*)(buf + (long)mdl->anims);
   mdl->meshes = (mesh_t*)(buf + (long)mdl->meshes);

   LOGI("Model: %s %ld bones %ld anims %ld meshes", mdl->name, mdl->nbones, mdl->nanims, mdl->nmeshes);

   long i = 0;

   bone_t* bone = &mdl->bones[0];
   for (i = 0; i < mdl->nbones; ++i)
   {
      LOGI("Bone: %s parent %d", bone->name, bone->parent);
      ++bone;
   }

   anim_t* anim = &mdl->anims[0];
   for (i = 0; i < mdl->nanims; ++i)
   {
      LOGI("Anim: %s [%ld frames]", anim->name, anim->nframes);
      anim->transforms = (float*)(buf + (long)anim->transforms);
      ++anim;
   }

   mesh_t* mesh = &mdl->meshes[0];
   for (i = 0; i < mdl->nmeshes; ++i)
   {
      LOGI("Mesh: %s [%ld vertices %ld indices]", mesh->name, mesh->nvertices, mesh->nindices);
      mesh->vertices = (vertex_t*)(buf + (long)mesh->vertices);
      mesh->indices = (int*)(buf + (long)mesh->indices);
      ++mesh;
   }

   (*m) = (model_t*)buf;

   return 0;
}

void model_free(const model_t* model)
{
   free((void*)model);
}

#define MAX_BONES 256
#define MAX_VERTICES 4096

extern GLuint gModelProgram;
extern GLuint gvPosHandle;
extern GLuint gvTexCoordHandle;
extern GLuint gvNormalHandle;
extern GLuint gvSampler;
extern GLuint gvMVP;
extern GLuint gvTextureId;

void model_render(const model_t* model, const cam_t* camera)
{
   mat4f_t bone_transforms[MAX_BONES];
   vertex_t vertices[MAX_VERTICES];
   long i = 0;
   long j = 0;

   long sequence = 0;
   long frame = 0;

   anim_t* anim = &model->anims[sequence];
   float* trans = anim->transforms + model->nbones*frame;

   bone_t* bone = &model->bones[0];
   for (i = 0; i < model->nbones; ++i)
   {
      vec4f_t quat;

      vec4f_t p;
      p.x = trans[0];
      p.y = trans[1];
      p.z = trans[2];
      p.w = 1.0f;

      vec4f_t q;
      q.x = trans[3];
      q.y = trans[4];
      q.z = trans[5];
      q.w = 1.0f - sqrt(q.x*q.x + q.y*q.y + q.z*q.z);

      quat_mult(&quat, &bone->quat, &q);

      mat4f_t transform;
      mat4_from_quaternion(&transform, &quat);
      transform.m14 = bone->pos.x + p.x;
      transform.m24 = bone->pos.y + p.y;
      transform.m34 = bone->pos.z + p.z;
      transform.m44 = 1.0f;

      if (bone->parent == -1)
      {
         bone_transforms[j] = transform;
      }
      else
      {
         mat4_mult(&bone_transforms[j], &bone_transforms[bone->parent], &transform);
      }

      LOGI("\nBone #%ld transform:", i);
      mat4_show(&transform);

      ++bone;
   }

   mesh_t* mesh = &model->meshes[0];
   for (i = 0; i < model->nmeshes; ++i)
   {
      // transform vertices
      for (j = 0; j < mesh->nvertices; ++j)
      {
         vertices[j] = mesh->vertices[j];
         mat4_mult_vector(&vertices[j].pos, &bone_transforms[mesh->vertices[j].bone[0]], &vertices[j].pos);
      }

      glVertexAttribPointer(gvPosHandle, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), &vertices[0].pos);
      glVertexAttribPointer(gvTexCoordHandle, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), &vertices[0].tex_coord);
      glVertexAttribPointer(gvNormalHandle, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), &vertices[0].normal);

      glEnableVertexAttribArray(gvPosHandle);
      glEnableVertexAttribArray(gvTexCoordHandle);
      glEnableVertexAttribArray(gvNormalHandle);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, gvTextureId);
      glUniform1i(gvSampler, 0);

      glCullFace(GL_FRONT);
      glDrawElements(GL_TRIANGLES, mesh->nindices, GL_UNSIGNED_INT, mesh->indices);

      ++mesh;
   }
}

