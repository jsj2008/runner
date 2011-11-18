-- vertex_shader
attribute vec3 aPos;
attribute vec2 aTexCoord;
uniform mat4 uMVP;
varying vec2 vTexCoord;

void main()
{
   vTexCoord = aTexCoord;
   gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}

-- pixel_shader
precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D uTex;

void main()
{
   gl_FragColor = texture2D(uTex, vTexCoord);
}

