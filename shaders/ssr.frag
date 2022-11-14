struct Buffers {
  sampler2D depth;
  sampler2D color;
  sampler2D blurredColor;
  sampler2D normal;
  sampler2D specular;
};

in vec2 fragTexcoords;

layout(std140) uniform uboCameraInfo {
  mat4 uniViewMat;
  mat4 uniInvViewMat;
  mat4 uniProjectionMat;
  mat4 uniInvProjectionMat;
  mat4 uniViewProjectionMat;
  vec3 uniCameraPos;
};

uniform Buffers uniSceneBuffers;

layout(location = 0) out vec4 fragColor;

const uint maxRaySteps    = 50u;
const uint maxRefineSteps = 10u;

vec2 refineRayHit(vec3 viewDir, vec3 viewPos) {
  for (uint refineStep = 0u; refineStep < maxRefineSteps; ++refineStep) {
    vec4 projPos = uniProjectionMat * vec4(viewPos, 1.0);
    projPos.xyz /= projPos.w;
    projPos.xyz  = projPos.xyz * 0.5 + 0.5;

    vec2 projCoords = projPos.xy;

    float projDepth = projPos.z;
    float fragDepth = texture(uniSceneBuffers.depth, projCoords).r;

    viewDir *= 0.5;

    if (projDepth > fragDepth)
      viewPos -= viewDir;
    else
      viewPos += viewDir;
  }

  vec4 projPos = uniProjectionMat * vec4(viewPos, 1.0);
  projPos.xy  /= projPos.w;

  vec2 projCoords = projPos.xy * 0.5 + 0.5;

  return projCoords;
}

vec2 recoverReflectTexcoords(vec3 viewDir, vec3 viewPos) {
  viewDir *= 0.5;

  for (uint rayStep = 0u; rayStep < maxRaySteps; ++rayStep) {
    viewPos += viewDir;

    vec4 projPos = uniProjectionMat * vec4(viewPos, 1.0);
    projPos.xyz /= projPos.w;
    projPos.xyz  = projPos.xyz * 0.5 + 0.5;

    vec2 projCoords = projPos.xy;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
      break; // The ray has gotten out of the frame, nothing will be found

    float projDepth = projPos.z;
    float fragDepth = texture(uniSceneBuffers.depth, projCoords).r;

    if (projDepth >= fragDepth)
      return refineRayHit(viewDir, viewPos);
  }

  return vec2(-1.0);
}

vec3 computeViewPosFromDepth(vec2 texcoords, float depth) {
  vec4 projPos = vec4(vec3(texcoords, depth) * 2.0 - 1.0, 1.0);
  vec4 viewPos = uniInvProjectionMat * projPos;

  return viewPos.xyz / viewPos.w;
}

vec3 computeFresnel(float cosTheta, vec3 baseReflectivity) {
  // Optimized exponent version, from: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
  return baseReflectivity + (1.0 - baseReflectivity) * pow(2.0, (-5.55473 * cosTheta - 6.98316) * cosTheta);
}

void main() {
  vec3 pixelColor = texture(uniSceneBuffers.color, fragTexcoords).rgb;
  float fragDepth = texture(uniSceneBuffers.depth, fragTexcoords).r;

  if (fragDepth == 1.0) {
    fragColor = vec4(pixelColor, 1.0);
    return;
  }

  vec3 viewFragPos = computeViewPosFromDepth(fragTexcoords, fragDepth);
  vec3 viewFragDir = normalize(viewFragPos); // The camera's position in view space is [0.0; 0.0; 0.0]

  vec3 worldNormal = normalize(texture(uniSceneBuffers.normal, fragTexcoords).rgb * 2.0 - 1.0);
  vec3 viewNormal  = normalize(mat3(uniViewMat) * worldNormal);

  vec3 viewReflectDir = normalize(reflect(viewFragDir, viewNormal));

  vec2 reflectTexcoords = recoverReflectTexcoords(viewReflectDir, viewFragPos);

  if (reflectTexcoords.x < 0.0 || reflectTexcoords.y < 0.0) {
    fragColor = vec4(pixelColor, 1.0);
    return;
  }

  vec4 specRough  = texture(uniSceneBuffers.specular, fragTexcoords);
  vec3 specular   = specRough.rgb;
  float roughness = specRough.a;

  float viewAngle = abs(dot(-viewFragDir, viewNormal));
  vec3 fresnel    = computeFresnel(viewAngle, specular);

  vec3 reflectColor        = texture(uniSceneBuffers.color, reflectTexcoords).rgb;
  vec3 reflectBlurredColor = texture(uniSceneBuffers.blurredColor, reflectTexcoords).rgb;

  fragColor = vec4(mix(pixelColor, mix(reflectColor, reflectBlurredColor, roughness), fresnel), 1.0);
}
