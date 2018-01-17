#version 330 core

uniform mat4 uniProjectionMatrix;
uniform mat4 uniInvProjMatrix;
uniform mat4 uniViewMatrix;
uniform mat4 uniInvViewMatrix;

uniform sampler2D uniSceneDepthBuffer;
uniform sampler2D uniSceneColorBuffer;
uniform sampler2D uniSceneNormalBuffer;

in vec2 fragTexcoords;

out vec4 fragColor;

const float rayStep = 0.5;
const uint maxRaySteps = 30u;
const uint maxBinaryRefinement = 5u;
const float maxThreshold = 1000.0;

/*vec3 hash(vec3 vec) {
  vec = fract(vec * vec3(0.8));
  vec += dot(vec, vec.yxz + 19.19);

  return fract((vec.xxy + vec.yxx) * vec.zyx);
}

vec2 binaryRefinement(vec3 dir, vec3 hitCoords) {
  vec4 projCoords;

  for (uint i = 0u; i < maxBinaryRefinement; ++i) {
    projCoords = uniProjectionMatrix * vec4(hitCoords, 1.0);
    projCoords.xy /= projCoords.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float depth = texture(uniSceneDepthBuffer, projCoords.xy).r;
    float depthDiff = hitCoords.z - depth;

    dir *= 0.5;

    if (depthDiff > 0.0)
      hitCoords += dir;
    else
      hitCoords -= dir;
  }

  projCoords = uniProjectionMatrix * vec4(hitCoords, 1.0);
  projCoords.xy /= projCoords.w;
  projCoords.xy = projCoords.xy * 0.5 + 0.5;

  return projCoords.xy;
}*/

vec2 recoverHitCoords(vec3 dir, vec3 hitCoords) {
  vec4 projCoords;

  dir *= rayStep;

  for (uint i = 0u; i < maxRaySteps; ++i) {
    hitCoords += dir;

    projCoords = uniProjectionMatrix * vec4(hitCoords, 1.0);
    projCoords.xy /= projCoords.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float depth = texture(uniSceneDepthBuffer, projCoords.xy).r;

    if (depth > maxThreshold)
      continue;

    float depthDiff = hitCoords.z - depth;

    if (dir.z - depthDiff < 1.2) {
      if (depthDiff >= 0.0)
        return projCoords.xy/*binaryRefinement(dir, hitCoords)*/;
    }
  }

  return projCoords.xy;
}

void main() {
  vec3 viewNormal = (uniInvProjMatrix * texture(uniSceneNormalBuffer, fragTexcoords)).rgb;

  float depth = texture(uniSceneDepthBuffer, fragTexcoords).r;
  vec3 projPos = vec3(fragTexcoords.x, 1.0 - fragTexcoords.y, depth) * 2.0 - 1.0;
  vec4 viewPos = uniInvProjMatrix * vec4(projPos, 1.0);
  viewPos /= viewPos.w;
  //vec3 worldPos = (uniInvViewMatrix * viewPos).xyz;

  //vec3 jittering = mix(vec3(0.0), vec3(hash(worldPos)), 1.0);

  vec3 reflectDir = reflect(normalize(viewPos.xyz), normalize(viewNormal));
  vec3 reflectColor = texture(uniSceneColorBuffer, recoverHitCoords(reflectDir * max(0.1, -viewPos.z), viewPos.xyz)).rgb;

  fragColor = vec4(reflectColor + texture(uniSceneColorBuffer, fragTexcoords).rgb, 1.0);
}
