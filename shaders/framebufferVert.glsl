#version 330 core

layout (location = 0) in vec2 vertPosition;
layout (location = 1) in vec2 vertTexcoords;

out vec2 fragTexcoords;

void main() {
  fragTexcoords = vertTexcoords;
  gl_Position = vec4(vertPosition.x, vertPosition.y, 0.0, 1.0);
}
