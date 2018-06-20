#version 330 core

in vec2 fragTexcoords;

uniform sampler2D uniTexture;

void main() {
  gl_FragColor = vec4(texture(uniTexture, fragTexcoords).rgb, 1.0);
}
