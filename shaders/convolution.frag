in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniInvBufferSize;
uniform float[9] uniKernel;

layout(location = 0) out vec4 fragColor;

// The following "matrix" is transposed compared to the common offsets list; this is due to the kernel matrix supposedly being sent as column-major,
//   thus not matching the usual indices used to access the offsets (kernel[i][j] would correspond to offset[j][i])
const vec2[9] offsets = vec2[](
  vec2(-1.0, 1.0), vec2(-1.0, 0.0), vec2(-1.0, -1.0),
  vec2( 0.0, 1.0), vec2( 0.0, 0.0), vec2( 0.0, -1.0),
  vec2( 1.0, 1.0), vec2( 1.0, 0.0), vec2( 1.0, -1.0)
);

void main() {
  vec3 result = vec3(0.0);

  for (int i = 0; i < 9; ++i) {
    vec2 offset = offsets[i] * uniInvBufferSize;
    result += texture(uniBuffer, fragTexcoords + offset).rgb * vec3(uniKernel[i]);
  }

  fragColor = vec4(result, 1.0);
}
