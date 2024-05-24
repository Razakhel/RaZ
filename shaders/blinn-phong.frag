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
layout(location = 2) out vec4 fragSpecular;

void main() {
  vec4 baseColor = texture(uniMaterial.baseColorMap, vertMeshInfo.vertTexcoords).rgba;
  float opacity  = texture(uniMaterial.opacityMap, vertMeshInfo.vertTexcoords).r;
  float alpha    = min(baseColor.a, opacity) * uniMaterial.opacity;

  if (alpha < 0.1)
      discard;

  vec3 color      = baseColor.rgb * uniMaterial.baseColor;
  vec3 specFactor = texture(uniMaterial.specularMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.specular;
  vec3 normal     = vertMeshInfo.vertTBNMatrix[2];
  vec3 viewDir    = normalize(uniCameraPos - vertMeshInfo.vertPosition);

  vec3 diffuse  = vec3(0.0);
  vec3 specular = vec3(0.0);

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
    vec3 radiance = uniLights[lightIndex].color.rgb * attenuation;

    // Diffuse
    float lightAngle = max(dot(lightDir, normal), 0.0);
    diffuse         += color * lightAngle * radiance;

    // Specular
    vec3 halfDir    = normalize(viewDir + lightDir);
    float halfAngle = max(dot(halfDir, normal), 0.0);
    specular       += specFactor * pow(halfAngle, 32.0) * radiance;
  }

  vec3 ambient    = color * 0.05;
  vec3 emissive   = texture(uniMaterial.emissiveMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;
  vec3 finalColor = ambient + diffuse + specular + emissive;

  // Gamma correction; this is temporary and will be removed later
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  fragColor    = vec4(finalColor, alpha);
  fragNormal   = normal * 0.5 + 0.5;
  fragSpecular = vec4(specFactor, 1.0 - max(specFactor.x, max(specFactor.y, specFactor.z)));
}
