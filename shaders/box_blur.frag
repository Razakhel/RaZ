in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniInvBufferSize;
uniform int uniKernelSize; // Technically half the size, but for name simplicity...

layout(location = 0) out vec4 fragColor;

void main() {
  vec3 result = vec3(0.0);

  for (int i = -uniKernelSize; i < uniKernelSize; ++i) {
    for (int j = -uniKernelSize; j < uniKernelSize; ++j) {
      vec2 offset = vec2(float(i), float(j)) * uniInvBufferSize;
      result += texture(uniBuffer, fragTexcoords + offset).rgb;
    }
  }

  int kernelFullSize = uniKernelSize * 2;
  fragColor = vec4(result / float(kernelFullSize * kernelFullSize), 1.0);
}
