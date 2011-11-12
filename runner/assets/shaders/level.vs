attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uMVI;
uniform vec3 uLightPos;
varying vec3 N;
varying vec3 L;
varying float vDist;
varying vec2 vTexCoord;

void main()
{
   vec3 point = (uMV * vec4(aPos.xyz, 1.0)).xyz;
   vec3 aux = uLightPos - point;
   vDist = length(aux);
   N = normalize(uMVI * vec4(aNormal.xyz, 0.0)).xyz;
   L = normalize(aux);

   vTexCoord = aTexCoord;
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}
