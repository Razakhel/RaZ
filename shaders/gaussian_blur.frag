in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniInvBufferSize;
uniform vec2 uniBlurDirection;

layout(location = 0) out vec4 fragColor;

const float kernelOffsets[3] = float[](0.0, 1.3846153846, 3.2307692308);
const float kernelWeights[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main() {
  vec3 blurredColor = texture(uniBuffer, gl_FragCoord.xy * uniInvBufferSize).rgb * kernelWeights[0];

  for (int i = 1; i < 3; ++i) {
    blurredColor += texture(uniBuffer, (gl_FragCoord.xy + kernelOffsets[i] * uniBlurDirection) * uniInvBufferSize).rgb * kernelWeights[i];
    blurredColor += texture(uniBuffer, (gl_FragCoord.xy - kernelOffsets[i] * uniBlurDirection) * uniInvBufferSize).rgb * kernelWeights[i];
  }

  fragColor = vec4(blurredColor, 1.0);
}
