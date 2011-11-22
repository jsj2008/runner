attribute vec3 aPos;attribute vec3 aColor;uniform mat4 uMVP;varying vec4 vColor;void main(){vColor=vec4(aColor,1.0);gl_Position=uMVP*vec4(aPos.xyz,1.0);}

precision mediump float;varying vec4 vColor;void main(){gl_FragColor=vColor;}



