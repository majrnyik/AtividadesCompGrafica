#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec4 lightPosWorldSpace;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;
out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragNObj;
out vec4 attenuation;

void main() {
  vec4 pWorld = modelMatrix * vec4(inPosition, 1.0);
  vec3 L = (viewMatrix * (lightPosWorldSpace - pWorld)).xyz;
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = (normalMatrix * inNormal);

  //calculo o fator de atenuacao, baseado no depth vert
  //L já é o vetor entre a fonte de luz e o vértice. Só preciso do tamanho do vetor
  //eu verifico se o tamanho é maior que 100 para impedir que a atenuação adote valores negativos
  float distanceFromLight = length(L);
  if(distanceFromLight > 100.0){
    distanceFromLight = 100.0;
  }
  float i = 1.0 - (distanceFromLight/ 100.0);

  fragL = L;
  fragV = -P;
  fragN = N;
  fragTexCoord = inTexCoord;
  fragPObj = inPosition;
  fragNObj = inNormal;
  attenuation = vec4(i, i, i, i);

  gl_Position = projMatrix * vec4(P, 1.0);
}
