attribute vec3 aPos;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
varying vec2 vTexCoord;

void main()
{
   vTexCoord = aTexCoord;
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

