in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniBufferSize;
uniform float uniStrength;

layout(location = 0) out vec4 fragColor;

void main() {
  vec2 uvScale = (1.0 - max(0.0001, uniStrength)) * uniBufferSize;
  vec2 uv      = round(fragTexcoords * uvScale) / uvScale;

  fragColor = vec4(texture(uniBuffer, uv).rgb, 1.0);
}
