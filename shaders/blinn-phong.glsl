#version 330 core

#define MAX_LIGHT_COUNT 10

struct Light {
  vec4 position;
  vec3 direction;
  vec3 color;
  float angle;
};

uniform uint uniLightCount;
uniform Light uniLights[MAX_LIGHT_COUNT];

uniform mat4 uniViewProjMatrix;

uniform struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 emissive;
  float transparency;

  sampler2D ambientMap;
  sampler2D diffuseMap;
  sampler2D specularMap;
  sampler2D transparencyMap;
  sampler2D bumpMap;
} uniMaterial;

in MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} fragMeshInfo;
in vec3 fragNormal;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 bufferNormal;

void main() {
  bufferNormal = normalize(fragNormal);

  vec3 normal   = fragMeshInfo.vertTBNMatrix[2];
  vec3 color    = texture(uniMaterial.diffuseMap, fragMeshInfo.vertTexcoords).rgb;
  vec3 ambient  = color * 0.05;
  vec3 diffuse  = vec3(0.0);
  vec3 specular = vec3(0.0);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 lightPos = (uniViewProjMatrix * uniLights[lightIndex].position).xyz;

    // Diffuse
    vec3 lightDir;

    if (uniLights[lightIndex].position.w != 0.0) {
      lightDir = normalize(lightPos - fragMeshInfo.vertPosition);
    } else {
      lightDir = normalize(-uniLights[lightIndex].direction);
    }

    diffuse += max(dot(lightDir, normal), 0.0) * color;

    // Specular
    vec3 viewDir = normalize(-fragMeshInfo.vertPosition);
    vec3 halfDir = normalize(lightDir + viewDir);
    specular += uniLights[lightIndex].color * pow(max(dot(normal, halfDir), 0.0), 32.0);
  }

  fragColor = vec4(ambient + diffuse + specular, 1.0);
}
