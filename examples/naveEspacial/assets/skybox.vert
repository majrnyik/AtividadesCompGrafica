#version 300 es

layout(location = 0) in vec3 inPosition;

out vec3 fragTexCoord;

void main() {
  fragTexCoord = inPosition;

  vec4 P = vec4(-1.0, -1.0, 1.0, 1.0);
  gl_Position = P.xyzw;
}