#define MAX_BONES 60

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
attribute vec2 aBoneIndices;
attribute vec2 aBoneWeights;
uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uBoneTransforms[MAX_BONES];
varying vec3 vColor;
varying vec4 vNormal;
varying vec2 vTexCoord;

void main()
{
   int index;

   vec4 newNormal;
   vec4 newPos;

   index = int(aBoneIndices.x);
   newPos = (uBoneTransforms[index] * vec4(aPos, 1.0)) * aBoneWeights.x;
   newNormal = (uBoneTransforms[index] * vec4(aNormal, 0.0)) * aBoneWeights.x;

   index = int(aBoneIndices.y);
   newPos = (uBoneTransforms[index] * vec4(aPos, 1.0)) * aBoneWeights.y + newPos;
   newNormal = (uBoneTransforms[index] * vec4(aNormal, 0.0)) * aBoneWeights.y + newNormal;

   vColor.x = clamp(aPos.x, 0.0, 1.0);
   vColor.y = clamp(aPos.y, 0.0, 1.0);
   vColor.z = clamp(aPos.z, 0.0, 1.0);

   vTexCoord = aTexCoord;
   vNormal = uMV * vec4(newNormal.xyz, 0.0);
   gl_Position = uMVP * vec4(newPos.xyz, 1.0);
}

