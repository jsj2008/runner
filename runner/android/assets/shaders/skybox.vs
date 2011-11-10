attribute vec4 aPos;
attribute vec2 aTexCoord;
attribute vec3 aColor;
varying vec2 vTexCoord;
varying vec3 vColor;

void main ()
{
   vTexCoord = aTexCoord;
   vColor = aColor;
   gl_Position = aPos;
}

