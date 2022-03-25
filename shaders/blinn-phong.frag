#define MAX_LIGHT_COUNT 10

struct Light {
  vec4 position;
  vec3 direction;
  float energy;
  vec3 color;
  float angle;
};

struct Material {
  vec3 diffuse;
  vec3 emissive;
  vec3 ambient;
  vec3 specular;
  float transparency;

  sampler2D diffuseMap;
  sampler2D emissiveMap;
  sampler2D ambientMap;
  sampler2D specularMap;
  sampler2D transparencyMap;
  sampler2D bumpMap;
};

in struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} vertMeshInfo;

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
  vec3 normal     = vertMeshInfo.vertTBNMatrix[2];
  vec3 color      = texture(uniMaterial.diffuseMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.diffuse;
  vec3 specFactor = texture(uniMaterial.specularMap, vertMeshInfo.vertTexcoords).r * uniMaterial.specular;

  vec3 ambient  = color * 0.05;
  vec3 diffuse  = vec3(0.0);
  vec3 specular = vec3(0.0);

  vec3 viewDir = normalize(cameraPos - vertMeshInfo.vertPosition);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    // Diffuse
    vec3 fullLightDir;
    float attenuation = uniLights[lightIndex].energy;

    if (uniLights[lightIndex].position.w != 0.0) {
      fullLightDir = uniLights[lightIndex].position.xyz - vertMeshInfo.vertPosition;

      float sqDist = dot(fullLightDir, fullLightDir);
      attenuation  /= sqDist;
    } else {
      fullLightDir = -uniLights[lightIndex].direction;
    }

    vec3 lightDir = normalize(fullLightDir);

    diffuse += max(dot(lightDir, normal), 0.0) * color * attenuation;

    // Specular
    vec3 halfDir = normalize(lightDir + viewDir);
    specular    += uniLights[lightIndex].color * pow(max(dot(halfDir, normal), 0.0), 32.0) * specFactor * attenuation;
  }

  vec3 emissive = texture(uniMaterial.emissiveMap, vertMeshInfo.vertTexcoords).rgb * uniMaterial.emissive;

  fragColor = vec4(ambient + diffuse + specular + emissive, specFactor);

  // Sending fragment normal to next framebuffer(s), if any
  bufferNormal = normal;
}
