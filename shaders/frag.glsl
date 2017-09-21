#version 330 core

uniform vec3 color = vec3(1.0f, 0.5f, 0.2f);

void main() {
  gl_FragColor = vec4(color, 1.0);
}
