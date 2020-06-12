#version 330 core

#define MAX_LIGHT_COUNT 10

struct Light {
  vec4 position;
  vec3 direction;
  float energy;
  vec3 color;
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

layout(std140) uniform uboCameraMatrices {
  mat4 viewMat;
  mat4 invViewMat;
  mat4 projectionMat;
  mat4 invProjectionMat;
  mat4 viewProjectionMat;
  vec3 cameraPos;
};

uniform Material uniMaterial;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 bufferNormal;

void main() {
  vec3 normal = fragMeshInfo.vertTBNMatrix[2];

  float lightHitAngle = 0.0;

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 lightPos = (viewProjectionMat * uniLights[lightIndex].position).xyz;
    vec3 lightDir;

    if (uniLights[lightIndex].position.w != 0.0) {
      lightDir = normalize(lightPos - fragMeshInfo.vertPosition);
    } else {
      lightDir = normalize(-uniLights[lightIndex].direction);
    }

    lightHitAngle = max(lightHitAngle, clamp(dot(lightDir, normal), 0.0, 1.0));
  }

  fragColor = vec4(lightHitAngle * texture(uniMaterial.diffuseMap, fragMeshInfo.vertTexcoords).rgb, 1.0);

  // Sending fragment normal to next framebuffer(s), if any
  bufferNormal = normal;
}
