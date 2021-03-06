#version 300 es

precision highp float;
precision highp int;

in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform int uniKernelSize; // Technically half the size, but for name simplicity...

layout(location = 0) out vec4 fragColor;

void main() {
  vec2 texelSize = 1.0 / vec2(textureSize(uniBuffer, 0));
  vec4 result    = vec4(0.0);

  for (int i = -uniKernelSize; i < uniKernelSize; ++i) {
    for (int j = -uniKernelSize; j < uniKernelSize; ++j) {
      vec2 offset = vec2(float(i), float(j)) * texelSize;
      result += texture(uniBuffer, fragTexcoords + offset).rgba;
    }
  }

  int kernelFullSize = uniKernelSize * 2;
  fragColor = result / float(kernelFullSize * kernelFullSize);
}
