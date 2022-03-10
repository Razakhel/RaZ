in vec2 fragTexcoords;

uniform sampler2D uniColorBuffer;
uniform float uniMaxWhite = 1.0;

layout(location = 0) out vec4 fragColor;

float computeLuminance(vec3 color) {
  return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 changeLuminance(vec3 color, float oldLuminance, float newLuminance) {
  return color * (newLuminance / oldLuminance);
}

vec3 computeReinhardExtendedLuminance(vec3 color) {
  float oldLuminance = computeLuminance(color);
  float numerator    = oldLuminance * ((oldLuminance / (uniMaxWhite * uniMaxWhite)) + 1.0);
  float newLuminance = numerator / (oldLuminance + 1.0);
  return changeLuminance(color, oldLuminance, newLuminance);
}

void main() {
  vec3 color = texture(uniColorBuffer, fragTexcoords).rgb;

  if (gl_FragCoord.x < 640) {
    if (gl_FragCoord.y < 360) // Bottom-left
      color = color / (color + vec3(1.0)); // Simple Reinhard
    else // Top-left
      color = (color * (color / (color + vec3(1.0)))) / computeLuminance(color); // Simple Reinhard with luminance
  } else {
    if (gl_FragCoord.y < 360) // Bottom-right
      color = (color * (color / (uniMaxWhite * uniMaxWhite) + vec3(1.0))) / (color + vec3(1.0)); // Extended Reinhard
    else // Top-right
      color = computeReinhardExtendedLuminance(color);// Extended Reinhard with luminance
  }

  fragColor = vec4(color, 1.0);
}
