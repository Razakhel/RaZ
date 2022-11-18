in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform float uniInvGamma;

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 color = texture(uniBuffer, fragTexcoords);
  fragColor  = vec4(pow(color.rgb, vec3(uniInvGamma)), color.a);
}
