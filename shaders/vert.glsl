#version 330 core

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 vertTexcoords;

out vec2 fragTexcoords;

void main() {
  gl_Position = vec4(vertPosition.xyz, 1.0);
  fragTexcoords = vertTexcoords;
}
