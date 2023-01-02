in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniInvBufferSize;
uniform float uniStrength;
uniform vec2 uniDirection;
uniform sampler2D uniMask;

layout(location = 0) out vec4 fragColor;

void main() {
  vec3 color  = texture(uniBuffer, fragTexcoords).rgb;
  vec2 offset = uniDirection * uniInvBufferSize * uniStrength;

  float red   = texture(uniBuffer, fragTexcoords - offset).r;
  float green = color.g;
  float blue  = texture(uniBuffer, fragTexcoords + offset).b;

  vec3 mask = texture(uniMask, fragTexcoords).rgb;

  fragColor = vec4(mix(color, vec3(red, green, blue), mask), 1.0);
}
