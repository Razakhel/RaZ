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

uniform sampler2D uniTexture;

in MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  vec3 vertNormal;
} fragMeshInfo;

void main() {
  vec3 norm = normalize(fragMeshInfo.vertNormal);
  float lightHitAngle = 0.0;

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 lightPos = (uniViewProjMatrix * uniLights[lightIndex].position).xyz;
    vec3 lightDir;

    if (uniLights[lightIndex].position.w == 0.0) {
      lightDir = normalize(lightPos - fragMeshInfo.vertPosition);
    } else {
      lightDir = uniLights[lightIndex].direction;
    }

    lightHitAngle = max(lightHitAngle, clamp(dot(lightDir, norm), 0.0, 1.0));
  }

  gl_FragColor = vec4(lightHitAngle * texture(uniTexture, fragMeshInfo.vertTexcoords).rgb, 1.0);
}
