#define MAX_LIGHT_COUNT 100
#define PI 3.1415926535897932384626433832795

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
  float metallicFactor;
  float roughnessFactor;

  sampler2D baseColorMap;
  sampler2D emissiveMap;
  sampler2D normalMap;
  sampler2D metallicMap;
  sampler2D roughnessMap;
  sampler2D ambientMap; // Ambient occlusion
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
layout(location = 2) out vec4 fragSpecular;

// Normal Distribution Function: Trowbridge-Reitz GGX
float computeNormalDistrib(vec3 normal, vec3 halfVec, float roughness) {
  float sqrRough  = roughness * roughness;
  float frthRough = sqrRough * sqrRough;

  float halfVecAngle    = max(dot(halfVec, normal), 0.0);
  float sqrHalfVecAngle = halfVecAngle * halfVecAngle;

  float divider = (sqrHalfVecAngle * (frthRough - 1.0) + 1.0);
  divider       = PI * divider * divider;

  return frthRough / max(divider, 0.001);
}

// Fresnel: Shlick
vec3 computeFresnel(float cosTheta, vec3 baseReflectivity) {
  // Optimized exponent version, from: http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
  return baseReflectivity + (1.0 - baseReflectivity) * pow(2.0, (-5.55473 * cosTheta - 6.98316) * cosTheta);
}

// Shlick-Beckmann for Geometry part
float computeGeomShlickGGX(float angle, float roughness) {
  float incrRough   = (roughness + 1.0);
  float roughFactor = (incrRough * incrRough) / 8.0;

  float denom = angle * (1.0 - roughFactor) + roughFactor;

  return angle / denom;
}

// Geometry: Smith's Shlick GGX
float computeGeometry(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
  float viewAngle  = max(dot(viewDir, normal), 0.0);
  float lightAngle = max(dot(lightDir, normal), 0.0);

  float viewGeom  = computeGeomShlickGGX(viewAngle, roughness);
  float lightGeom = computeGeomShlickGGX(lightAngle, roughness);

  return viewGeom * lightGeom;
}

void main() {
  vec4 baseColor = texture(uniMaterial.baseColorMap, vertMeshInfo.vertTexcoords).rgba;

  if (baseColor.a < 0.1)
    discard;

  vec3 albedo     = baseColor.rgb * uniMaterial.baseColor;
  float metallic  = texture(uniMaterial.metallicMap, vertMeshInfo.vertTexcoords).r * uniMaterial.metallicFactor;
  float roughness = texture(uniMaterial.roughnessMap, vertMeshInfo.vertTexcoords).r * uniMaterial.roughnessFactor;
  float ambOcc    = texture(uniMaterial.ambientMap, vertMeshInfo.vertTexcoords).r;

  vec3 normal = texture(uniMaterial.normalMap, vertMeshInfo.vertTexcoords).rgb;
  normal      = normalize(normal * 2.0 - 1.0);
  normal      = normalize(vertMeshInfo.vertTBNMatrix * normal);

  vec3 viewDir = normalize(uniCameraPos - vertMeshInfo.vertPosition);

  // Base Fresnel (F0)
  vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);

  vec3 albedoFactor = albedo / PI;

  vec3 lightRadiance = vec3(0.0);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 fullLightDir;
    float attenuation = uniLights[lightIndex].energy;

    if (uniLights[lightIndex].position.w != 0.0) {
      fullLightDir = uniLights[lightIndex].position.xyz - vertMeshInfo.vertPosition;

      float sqDist = dot(fullLightDir, fullLightDir);
      attenuation /= sqDist;
    } else {
      fullLightDir = -uniLights[lightIndex].direction.xyz;
    }

    vec3 lightDir = normalize(fullLightDir);
    vec3 halfDir  = normalize(viewDir + lightDir);
    vec3 radiance = uniLights[lightIndex].color.rgb * attenuation;

    // Normal distribution (D)
    float normalDistrib = computeNormalDistrib(normal, halfDir, roughness);

    // Fresnel (F)
    vec3 fresnel = computeFresnel(max(dot(halfDir, viewDir), 0.0), baseReflectivity);

    // Geometry (G)
    float geometry = computeGeometry(normal, viewDir, lightDir, roughness);

    vec3 DFG         = normalDistrib * fresnel * geometry;
    float lightAngle = max(dot(lightDir, normal), 0.0);
    float divider    = 4.0 * max(dot(viewDir, normal), 0.0) * lightAngle;
    vec3 specular    = DFG / max(divider, 0.001);

    vec3 diffuse = vec3(1.0) - fresnel;
    diffuse     *= 1.0 - metallic;

    lightRadiance += (diffuse * albedoFactor + specular) * radiance * lightAngle;
  }

  vec3 ambient    = vec3(0.03) * albedo * ambOcc;
  vec3 emissive   = texture(uniMaterial.emissiveMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;
  vec3 finalColor = ambient + lightRadiance + emissive;

  // Reinhard tone mapping; this is temporary and will be removed later
  finalColor = finalColor / (finalColor + vec3(1.0));
  // Gamma correction; this is temporary and will be removed later
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragColor    = vec4(finalColor, baseColor.a);
  fragNormal   = normal * 0.5 + 0.5;
  fragSpecular = vec4(baseReflectivity, roughness);
}
