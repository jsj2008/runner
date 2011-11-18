-- vertex_shader
attribute vec3 aPos;
uniform mat4 uMVP;

void main()
{
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

-- pixel_shader
precision mediump float;

void main()
{
   gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}

