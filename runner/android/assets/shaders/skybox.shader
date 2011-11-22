attribute vec4 aPos;attribute vec2 aTexCoord;attribute vec3 aColor;varying vec2 vTexCoord;varying vec3 vColor;void main(){vTexCoord=aTexCoord;vColor=aColor;gl_Position=aPos;}

precision mediump float;varying vec3 vColor;varying vec2 vTexCoord;uniform sampler2D uTex;void main(){gl_FragColor=texture2D(uTex,vTexCoord)*vec4(vColor,1.0);}



