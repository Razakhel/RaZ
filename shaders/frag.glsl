#version 330 core

uniform sampler2D uniTexture;

in vec2 fragTexcoords;

void main() {
  gl_FragColor = texture(uniTexture, fragTexcoords);
}
