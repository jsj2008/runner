attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uMVI;
varying vec3 N;
varying vec3 vPoint;
varying vec2 vTexCoord;

void main()
{
   vPoint = (uMV * vec4(aPos.xyz, 1.0)).xyz;
   N = normalize(uMVI * vec4(aNormal.xyz, 0.0)).xyz;

   vTexCoord = aTexCoord;
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

