#version 330 core

struct Buffers {
  sampler2D depth;
  sampler2D normal;
  sampler2D color;
};

in vec2 fragTexcoords;

layout (std140) uniform uboCameraMatrices {
  mat4 viewMat;
  mat4 invViewMat;
  mat4 projectionMat;
  mat4 invProjectionMat;
  mat4 viewProjectionMat;
  vec3 cameraPos;
};

uniform Buffers uniSceneBuffers;

layout (location = 0) out vec4 fragColor;

const uint maxRaySteps    = 500u;
const uint maxBinarySteps = 5u;

vec3 binaryRefinement(vec3 rayDir, vec3 viewPos) {
  vec4 projCoords;

  for (uint binStep = 0u; binStep < maxBinarySteps; ++binStep) {
    projCoords     = projectionMat * vec4(viewPos, 1.0);
    projCoords.xy /= projCoords.w;
    projCoords.xy  = projCoords.xy * 0.5 + 0.5;

    float depth     = texture(uniSceneBuffers.depth, projCoords.xy).r;
    float depthDiff = viewPos.z - depth;

    rayDir *= 0.5;

    if (depthDiff > 0.0)
      viewPos += rayDir;
    else
      viewPos -= rayDir;
  }

  projCoords     = projectionMat * vec4(viewPos, 1.0);
  projCoords.xy /= projCoords.w;
  projCoords.xy  = projCoords.xy * 0.5 + 0.5;

  return texture(uniSceneBuffers.color, projCoords.xy).rgb;
}

vec3 recoverReflectColor(vec3 rayDir, vec3 viewPos) {
  rayDir *= 0.5;

  for (uint rayStep = 0u; rayStep < maxRaySteps; ++rayStep) {
    viewPos += rayDir;

    vec4 projCoords = projectionMat * vec4(viewPos, 1.0);
    projCoords.xy  /= projCoords.w;
    projCoords.xy   = projCoords.xy * 0.5 + 0.5;

    float depth     = texture(uniSceneBuffers.depth, projCoords.xy).r;
    float depthDiff = viewPos.z - depth;

    if (depthDiff < 0.0)
      return texture(uniSceneBuffers.color, projCoords.xy).rgb;
      //return binaryRefinement(rayDir, viewPos);
  }

  return vec3(0.0);
}

vec3 computeViewPosFromDepth(vec2 texcoords, float depth) {
  vec4 projPos = vec4(vec3(texcoords, depth) * 2.0 - 1.0, 1.0);
  vec4 viewPos = invProjectionMat * projPos;

  return viewPos.xyz / viewPos.w;
}

void main() {
  vec3 reflectColor   = texture(uniSceneBuffers.color, fragTexcoords).rgb;
  float reflectFactor = texture(uniSceneBuffers.color, fragTexcoords).a;

  if (reflectFactor > 0.1) {
    float depth = texture(uniSceneBuffers.depth, fragTexcoords).r;

    vec3 viewPos     = computeViewPosFromDepth(fragTexcoords, depth);
    vec3 worldNormal = normalize(texture(uniSceneBuffers.normal, fragTexcoords).xyz * 2.0 - 1.0);
    vec3 reflectDir  = normalize(reflect(normalize(viewPos), worldNormal));

    reflectColor *= 1.0 - reflectFactor;
    reflectColor += recoverReflectColor(vec3(reflectDir.xy, -reflectDir.z) * max(0.1, -viewPos.z), viewPos.xyz) * reflectFactor;
  }

  fragColor = vec4(reflectColor.xyz, 1.0);
}
