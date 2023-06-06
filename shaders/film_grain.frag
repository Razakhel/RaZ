in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform float uniStrength;

layout(std140) uniform uboTimeInfo {
  float uniDeltaTime;
  float uniGlobalTime;
};

layout(location = 0) out vec4 fragColor;

float hash(vec2 vec) {
  // "Hash without Sine", from https://www.shadertoy.com/view/4djSRW
  vec3 v3 = fract(vec3(vec.xyx) * 0.1031);
  v3     += dot(v3, v3.yzx + 33.33);
  return fract((v3.x + v3.y) * v3.z);
}

void main() {
  float grain = (hash(fragTexcoords * vec2(312.24, 1030.057) * (uniGlobalTime + 1.0)) * 2.0 - 1.0) * uniStrength;
  fragColor   = vec4(texture(uniBuffer, fragTexcoords).rgb + vec3(grain), 1.0);
}
