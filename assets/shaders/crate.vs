#define MAX_BONES 60

attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
uniform mat4 uMV;
varying vec4 vNormal;
varying vec2 vTexCoord;

void main()
{
   vTexCoord = aTexCoord;
   vNormal = uMV * vec4(aNormal.xyz, 0.0);
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

