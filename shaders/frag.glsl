#version 330 core

uniform sampler2D uniTexture;

in vec2 fragTexcoords;
in float fragLightHitAngle;

void main() {
  gl_FragColor = fragLightHitAngle * texture(uniTexture, fragTexcoords);
}
