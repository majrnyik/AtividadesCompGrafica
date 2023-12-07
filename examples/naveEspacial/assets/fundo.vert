#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(inPosition.x, inPosition.y, 0.5, 1.0);
    //não consegui nem pelo poder de charls babbage passar o inTexCoord
    //vou calcular na mão e chumbar aqui as coordenadas de textura do meu fundo
    fragTexCoord = vec2((inPosition.x + 1)/2, (inPosition.y + 1)/2);
}