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
const uint ssaoCount = 128u;
const float goldenRatio = (2.236f + 1.0f) / 2.0f;

#define PI 3.1415926535897932384626433832795

/*vec3 hash(vec3 vec) {
  vec = fract(vec * vec3(0.8));
  vec += dot(vec, vec.yxz + 19.19);

  return fract((vec.xxy + vec.yxx) * vec.zyx);
}*/

float rand(vec2 vec) {
  return fract(sin(dot(vec.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float copysign(float a, float b) {
  a = abs(a);

  if (b < 0)
    a = -a;

  return a;
}

void branchlessONB(vec3 normal, out vec3 b1, out vec3 b2) {
  float sign = copysign(1.0f, normal.z);
  float a = -1.0f / (sign + normal.z);
  float b = normal.x * normal.y * a;

  b1 = vec3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  b2 = vec3(b, sign + normal.y * normal.y * a, -normal.y);
}

vec3 fiboDir(vec3 worldNormal, uint i) {
  float rand = (i + rand(worldNormal.xz)) / goldenRatio;

  float phi = 2.0f * PI * (rand - floor(rand));
  float cosTheta = 1.0f - ((2.0f * i + 1.0f) / (2.0f * ssaoCount));
  float sinTheta = sqrt(1.0f - (cosTheta * cosTheta));

  vec3 direction = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

  vec3 b1, b2;
  branchlessONB(worldNormal, b1, b2);
  return direction.x * b1 + direction.y * b2 + direction.z * worldNormal;
}

float ssao(vec3 worldPos, vec3 worldNormal, float depth) {
  vec3 rayOrig = worldPos + 0.001f * worldNormal;
  float nbHits = 0.0;

  for (uint i = 0u; i < ssaoCount; ++i) {
    vec3 fiboDir = fiboDir(worldNormal, i);
    float rand = rand(worldPos.xz * i) * 50;
    vec3 rayStep = rayOrig + rand * fiboDir;

    vec4 projCoords = uniProjectionMatrix * uniViewMatrix * vec4(rayStep, 1.0);
    vec2 hitCoords = projCoords.xy / projCoords.w * 0.5 + 0.5;
    float hitDepth = texture(uniSceneDepthBuffer, hitCoords).r;

    if (hitDepth > depth)
      ++nbHits;
  }

  float coeff = nbHits / ssaoCount;

  return clamp(pow(coeff, 0.5), 0, 1);
}

/*vec2 binaryRefinement(vec3 dir, vec3 hitCoords) {
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
  vec3 viewNormal = normalize((uniInvProjMatrix * texture(uniSceneNormalBuffer, fragTexcoords)).rgb);

  float depth = texture(uniSceneDepthBuffer, fragTexcoords).r;
  vec3 projPos = vec3(fragTexcoords.x, 1.0 - fragTexcoords.y, depth) * 2.0 - 1.0;
  vec4 viewPos = uniInvProjMatrix * vec4(projPos, 1.0);
  viewPos /= viewPos.w;
  vec3 worldPos = (uniInvViewMatrix * viewPos).xyz;

  //vec3 jittering = mix(vec3(0.0), vec3(hash(worldPos)), 1.0);

  vec3 reflectDir = reflect(normalize(viewPos.xyz), viewNormal);
  vec3 reflectColor = texture(uniSceneColorBuffer, recoverHitCoords(reflectDir * max(0.1, -viewPos.z), viewPos.xyz)).rgb;
  float ssaoFactor = ssao(worldPos, normalize(uniInvViewMatrix * vec4(viewNormal, 1.0)).xyz, depth);

  fragColor = vec4(reflectColor + ssaoFactor * texture(uniSceneColorBuffer, fragTexcoords).rgb, 1.0);
}
