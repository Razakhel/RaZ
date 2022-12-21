in vec2 fragTexcoords;

uniform sampler2D uniBuffer;
uniform float uniFrameRatio;
uniform float uniStrength;
uniform float uniOpacity;
uniform vec3 uniColor;

layout(location = 0) out vec4 fragColor;

void main() {
  // Natural vignetting/illumination falloff, using the cosine fourth law. See:
  // - https://www.shadertoy.com/view/4lSXDm
  // - https://github.com/keijiro/KinoVignette/blob/master/Assets/Kino/Vignette/Shader/Vignette.shader
  // - https://en.wikipedia.org/wiki/Vignetting#Natural_vignetting
  // - https://www.cs.cmu.edu/~sensing-sensors/readings/vignetting.pdf#page=3

  vec2 uv     = (fragTexcoords - 0.5) * vec2(uniFrameRatio, 1.0) * 2.0;
  float len   = length(uv) * uniStrength;
  float sqLen = len * len + 1.0;
  float fade  = 1.0 / (sqLen * sqLen);

  // The following implementation may be another solution, but produces hard borders. See:
  // - https://www.shadertoy.com/view/lsKSWR
  // - https://godotshaders.com/shader/color-vignetting/

  //vec2 uv    = fragTexcoords * (1.0 - fragTexcoords);
  //float fade = uv.x * uv.y * 15.0;
  //fade       = pow(fade, uniStrength);

  vec3 color      = texture(uniBuffer, fragTexcoords).rgb;
  vec3 fadedColor = mix(color, uniColor, 1.0 - fade);

  fragColor = vec4(mix(color, fadedColor, uniOpacity), 1.0);
}
