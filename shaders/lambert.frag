#define MAX_LIGHT_COUNT 100

struct Light {
  vec4 position;
  vec4 direction;
  vec4 color;
  float energy;
  float angle;
};

struct Material {
  vec3 baseColor;
  vec3 emissive;
  vec3 ambient;
  vec3 specular;
  float opacity;

  sampler2D baseColorMap;
  sampler2D emissiveMap;
  sampler2D ambientMap;
  sampler2D specularMap;
  sampler2D opacityMap;
  sampler2D bumpMap;
};

in struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} vertMeshInfo;

layout(std140) uniform uboCameraInfo {
  mat4 uniViewMat;
  mat4 uniInvViewMat;
  mat4 uniProjectionMat;
  mat4 uniInvProjectionMat;
  mat4 uniViewProjectionMat;
  vec3 uniCameraPos;
};

layout(std140) uniform uboLightsInfo {
  Light uniLights[MAX_LIGHT_COUNT];
  uint uniLightCount;
};

uniform Material uniMaterial;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;

void main() {
  vec4 baseColor = texture(uniMaterial.baseColorMap, vertMeshInfo.vertTexcoords).rgba;
  float opacity  = texture(uniMaterial.opacityMap, vertMeshInfo.vertTexcoords).r;
  float alpha    = min(baseColor.a, opacity) * uniMaterial.opacity;

  if (alpha < 0.1)
    discard;

  vec3 normal = vertMeshInfo.vertTBNMatrix[2];

  float lightHitAngle = 0.0;

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 lightPos = (uniViewProjectionMat * uniLights[lightIndex].position).xyz;
    vec3 lightDir;

    if (uniLights[lightIndex].position.w != 0.0)
      lightDir = normalize(lightPos - vertMeshInfo.vertPosition);
    else
      lightDir = normalize(-uniLights[lightIndex].direction.xyz);

    lightHitAngle = max(lightHitAngle, clamp(dot(lightDir, normal), 0.0, 1.0));
  }

  vec3 diffuse    = lightHitAngle * baseColor.rgb * uniMaterial.baseColor;
  vec3 emissive   = texture(uniMaterial.emissiveMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;
  vec3 finalColor = diffuse + emissive;

  // Gamma correction; this is temporary and will be removed later
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragColor  = vec4(finalColor, alpha);
  fragNormal = normal;
}
