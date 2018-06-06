#version 330 core

#define MAX_LIGHT_COUNT 10

struct Light {
  vec4 position;
  vec3 direction;
  vec3 color;
  float energy;
  float angle;
};

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 emissive;
  float transparency;

  sampler2D ambientMap;
  sampler2D diffuseMap;
  sampler2D specularMap;
  sampler2D emissiveMap;
  sampler2D transparencyMap;
  sampler2D bumpMap;
};

in MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} fragMeshInfo;

uniform uint uniLightCount;
uniform Light uniLights[MAX_LIGHT_COUNT];

uniform vec3 uniCameraPos;

uniform Material uniMaterial;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 bufferNormal;

void main() {
  vec3 normal     = fragMeshInfo.vertTBNMatrix[2];
  vec3 color      = texture(uniMaterial.diffuseMap, fragMeshInfo.vertTexcoords).rgb * uniMaterial.diffuse;
  vec3 specFactor = texture(uniMaterial.specularMap, fragMeshInfo.vertTexcoords).r * uniMaterial.specular;

  vec3 ambient  = color * 0.05;
  vec3 diffuse  = vec3(0.0);
  vec3 specular = vec3(0.0);

  vec3 viewDir = normalize(uniCameraPos - fragMeshInfo.vertPosition);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    // Diffuse
    vec3 fullLightDir;
    float attenuation = uniLights[lightIndex].energy;

    if (uniLights[lightIndex].position.w != 0.0) {
      fullLightDir = uniLights[lightIndex].position.xyz - fragMeshInfo.vertPosition;

      float sqrDist = dot(fullLightDir, fullLightDir);
      attenuation  /= sqrDist;
    } else {
      fullLightDir = -uniLights[lightIndex].direction;
    }

    vec3 lightDir = normalize(fullLightDir);

    diffuse += max(dot(lightDir, normal), 0.0) * color * attenuation;

    // Specular
    vec3 halfDir = normalize(lightDir + viewDir);
    specular    += uniLights[lightIndex].color * pow(max(dot(halfDir, normal), 0.0), 32.0) * specFactor * attenuation;
  }

  vec3 emissive = texture(uniMaterial.emissiveMap, fragMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;

  fragColor = vec4(ambient + diffuse + specular + emissive, specFactor);

  // Sending fragment normal to next framebuffer(s), if any
  bufferNormal = normal;
}
