in vec2 fragTexcoords;

uniform sampler2D uniGradients;
uniform sampler2D uniGradDirs;
uniform vec2 uniInvBufferSize;
uniform float uniLowerBound;
uniform float uniUpperBound;

layout(location = 0) out vec4 fragColor;

void main() {
  vec3 midGrad = texture(uniGradients, fragTexcoords).rgb;
  vec3 gradDir = texture(uniGradDirs, fragTexcoords).rgb;

  vec3 rightGrad      = texture(uniGradients, fragTexcoords + vec2( 1.0,  0.0) * uniInvBufferSize).rgb;
  vec3 leftGrad       = texture(uniGradients, fragTexcoords + vec2(-1.0,  0.0) * uniInvBufferSize).rgb;
  vec3 downGrad       = texture(uniGradients, fragTexcoords + vec2( 0.0, -1.0) * uniInvBufferSize).rgb;
  vec3 upGrad         = texture(uniGradients, fragTexcoords + vec2( 0.0,  1.0) * uniInvBufferSize).rgb;
  vec3 lowerRightGrad = texture(uniGradients, fragTexcoords + vec2( 1.0, -1.0) * uniInvBufferSize).rgb;
  vec3 upperLeftGrad  = texture(uniGradients, fragTexcoords + vec2(-1.0,  1.0) * uniInvBufferSize).rgb;
  vec3 lowerLeftGrad  = texture(uniGradients, fragTexcoords + vec2(-1.0, -1.0) * uniInvBufferSize).rgb;
  vec3 upperRightGrad = texture(uniGradients, fragTexcoords + vec2( 1.0,  1.0) * uniInvBufferSize).rgb;

  for (int i = 0; i < 3; ++i) {
    // Merging the two directions' halves together; we want to check opposite directions each time, and both will be combined that way
    //
    //           /--0.75--\
    //         /            \
    //       /                \
    //     0.5                0/1
    //       \                /
    //         \            /
    //           \--0.25--/
    //
    // 0.6 (upper-left) will become 0.1 (lower-right), 0.75 (up) will become 0.25 (down), etc
    if (gradDir[i] > 0.5)
      gradDir[i] -= 0.5;

    // Gradient magnitude thresholding (edge thinning)

    if (gradDir[i] <= 0.0625 || gradDir[i] > 0.4375) { // Right or left
      if (midGrad[i] < rightGrad[i] || midGrad[i] < leftGrad[i])
        midGrad[i] = 0.0;
    } else if (gradDir[i] > 0.1875 && gradDir[i] <= 0.3125) { // Down or up
      if (midGrad[i] < downGrad[i] || midGrad[i] < upGrad[i])
        midGrad[i] = 0.0;
    } else if (gradDir[i] > 0.0625 && gradDir[i] <= 0.1875) { // Lower-right or upper-left
      if (midGrad[i] < lowerRightGrad[i] || midGrad[i] < upperLeftGrad[i])
        midGrad[i] = 0.0;
    } else if (gradDir[i] > 0.3125 && gradDir[i] <= 0.4375) { // Lower-left or upper-right
      if (midGrad[i] < lowerLeftGrad[i] || midGrad[i] < upperRightGrad[i])
        midGrad[i] = 0.0;
    }

    // Double thresholding + hysteresis

    if (midGrad[i] <= uniLowerBound) {
      midGrad[i] = 0.0;
    } else if (midGrad[i] >= uniUpperBound) {
      midGrad[i] = 1.0;
    } else {
      // If the pixel is on a weak edge (between bounds), refining it with hysteresis: if any pixel around the current one is on a strong edge
      //  (has a gradient value above the upper bound), consider it as part of the edge
      if (rightGrad[i] >= uniUpperBound || leftGrad[i] >= uniUpperBound
       || downGrad[i] >= uniUpperBound || upGrad[i] >= uniUpperBound
       || lowerRightGrad[i] >= uniUpperBound || upperLeftGrad[i] >= uniUpperBound
       || lowerLeftGrad[i] >= uniUpperBound || upperRightGrad[i] >= uniUpperBound) {
        midGrad[i] = 1.0;
      } else {
        midGrad[i] = 0.0;
      }
    }
  }

  float colorVal = max(midGrad.x, max(midGrad.y, midGrad.z));
  fragColor      = vec4(vec3(colorVal), 1.0);
}
