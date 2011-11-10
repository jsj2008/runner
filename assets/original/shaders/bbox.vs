attribute vec3 aPos;
uniform mat4 uMVP;

void main()
{
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

