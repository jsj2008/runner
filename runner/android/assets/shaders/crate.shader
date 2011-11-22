attribute vec3 aPos;attribute vec3 aNormal;attribute vec2 aTexCoord;uniform mat4 uMVP;uniform mat4 uMV;uniform mat4 uMVI;uniform vec3 uLightPos;varying vec3 N;varying vec3 L;varying vec3 E;varying vec3 R;varying float vDist;varying vec2 vTexCoord;void main(){vec3 point=(uMV*vec4(aPos.xyz,1.0)).xyz;vec3 aux=uLightPos-point;vDist=length(aux);N=normalize(uMVI*vec4(aNormal.xyz,0.0)).xyz;L=normalize(aux);E=normalize(-point);R=normalize(-reflect(L,N));vTexCoord=aTexCoord;gl_Position=uMVP*vec4(aPos.xyz,1.0);}

precision mediump float;varying vec3 N;varying vec3 L;varying vec3 E;varying vec3 R;varying vec2 vTexCoord;varying float vDist;uniform sampler2D uTex;uniform vec3 uMatDiffuse;uniform vec3 uMatSpecular;uniform float uMatShininess;void main(){const float falloffDistance=30.0;float lambertTerm=max(dot(N,L),0.0);float coeff=vDist/falloffDistance;float attenuation=clamp(1.0-coeff*coeff,0.0,1.0);float diffuse=min(lambertTerm*attenuation+0.5,1.0);vec4 color=vec4(uMatDiffuse.rgb,1.0)*texture2D(uTex,vTexCoord)*diffuse;float specular=pow(max(0.0,dot(R,E)),uMatShininess);color+=vec4(uMatSpecular.rgb,1.0)*specular;gl_FragColor=color;}



