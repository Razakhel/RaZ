#define PI 3.1415926535897932384626433832795

in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform vec2 uniInvBufferSize;

layout(location = 0) out vec4 fragGradient;
layout(location = 1) out vec4 fragGradDir;

const float[9] horizKernel = float[](
  1.0, 0.0, -1.0,
  2.0, 0.0, -2.0,
  1.0, 0.0, -1.0
);

const float[9] vertKernel = float[](
   1.0,  2.0,  1.0,
   0.0,  0.0,  0.0,
  -1.0, -2.0, -1.0
);

const vec2[9] offsets = vec2[](
  vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2(1.0,  1.0),
  vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2(1.0,  0.0),
  vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0)
);

const float invTau = 1.0 / (2.0 * PI);

void main() {
  vec3 horizVal = vec3(0.0);
  vec3 vertVal  = vec3(0.0);

  for (int i = 0; i < 9; ++i) {
    vec2 offset = offsets[i] * uniInvBufferSize;
    vec3 pixel  = texture(uniBuffer, fragTexcoords + offset).rgb;

    horizVal += pixel * horizKernel[i];
    vertVal  += pixel * vertKernel[i];
  }

  vec3 gradient = sqrt(horizVal * horizVal + vertVal * vertVal);
  vec3 gradDir  = atan(vertVal, horizVal);
  gradDir       = (gradDir + PI) * invTau; // Remapping from [-Pi; Pi] to [0; 1]

  fragGradient = vec4(gradient, 1.0);
  fragGradDir  = vec4(gradDir, 1.0);
}
