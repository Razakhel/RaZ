struct Buffers {
  sampler2D depth;
  sampler2D color;
  sampler2D normal;
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

const uint ssaoCount    = 32u;
const float goldenRatio = (2.236 + 1.0) / 2.0;

#define PI 3.1415926535897932384626433832795

float rand(vec2 vec) {
  return fract(sin(dot(vec.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float copysign(float a, float b) {
  a = abs(a);

  if (b < 0)
    a = -a;

  return a;
}

void buildOrthoBase(vec3 normal, out vec3 xAxis, out vec3 zAxis) {
  float sign = copysign(1.0, normal.z);
  float a    = -1.0 / (sign + normal.z);
  float b    = normal.x * normal.y * a;

  xAxis = vec3(1.0 + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  zAxis = vec3(b, sign + normal.y * normal.y * a, -normal.y);
}

vec3 computeFiboDir(vec3 normal, uint raySample) {
  float cosTheta = 1.0 - ((2.0 * float(raySample) + 1.0) / (2.0 * float(ssaoCount)));
  float sinTheta = sqrt(1.0 - (cosTheta * cosTheta));

  float randVal = (raySample + rand(normal.xz)) / goldenRatio;
  float phi     = 2.0 * PI * (randVal - floor(randVal));

  vec3 direction = vec3(cos(phi) * sinTheta, cosTheta, sin(phi) * sinTheta);

  vec3 xAxis, zAxis;
  buildOrthoBase(normal, xAxis, zAxis);

  return direction.x * xAxis + direction.y * normal + direction.z * zAxis;
}

float computeSsaoFactor(vec3 viewPos, vec3 worldNormal, float depth) {
  vec3 rayOrig   = viewPos + worldNormal * 0.0001;
  float hitCount = 0.0;

  for (uint raySample = 0u; raySample < ssaoCount; ++raySample) {
    vec3 rayDir   = computeFiboDir(worldNormal, raySample);
    float randVal = rand(viewPos.xy * raySample) * 50;
    vec3 rayStep  = rayOrig + randVal * rayDir;

    vec4 projCoords = uniProjectionMat * vec4(rayStep, 1.0);
    projCoords     /= projCoords.w;

    vec2 hitCoords = projCoords.xy * 0.5 + 0.5;
    float hitDepth = texture(uniSceneBuffers.depth, hitCoords).r;

    if (hitDepth > depth)
      ++hitCount;
  }

  float coeff = hitCount / ssaoCount;
  return clamp(pow(coeff, 0.5), 0, 1);
}

vec3 computeViewPosFromDepth(vec2 texcoords, float depth) {
  vec4 projPos = vec4(vec3(texcoords, depth) * 2.0 - 1.0, 1.0);
  vec4 viewPos = uniInvProjectionMat * projPos;

  return viewPos.xyz / viewPos.w;
}

void main() {
  float depth = texture(uniSceneBuffers.depth, fragTexcoords).r;

  vec3 viewPos  = computeViewPosFromDepth(fragTexcoords, depth);
  vec3 worldPos = vec3(uniInvViewMat * vec4(viewPos, 1.0));

  vec3 worldNormal = normalize(texture(uniSceneBuffers.normal, fragTexcoords).xyz * 2.0 - 1.0);

  if (gl_FragCoord.x < 640) {
    float ssaoFactor = computeSsaoFactor(viewPos, worldNormal, depth);
    fragColor = vec4(vec3(ssaoFactor), 1.0);
  } else {
    fragColor = vec4(texture(uniSceneBuffers.color, fragTexcoords).rgb, 1.0);
  }
}
