attribute vec4 aPos;
attribute vec4 aNormal;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
varying vec3 vColor;
varying vec4 vNormal;
varying vec2 vTexCoord;

void main()
{
   vTexCoord = aTexCoord;
   vNormal = aNormal;
   vColor.x = clamp(aPos.x, 0.0, 1.0);
   vColor.y = clamp(aPos.y, 0.0, 1.0);
   vColor.z = clamp(aPos.z, 0.0, 1.0);
   gl_Position = uMVP * aPos;
}

