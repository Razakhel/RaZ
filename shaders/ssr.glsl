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

uniform mat4 uniProjectionMatrix;
uniform mat4 uniViewProjMatrix;

uniform sampler2D uniTexture;
uniform sampler2D uniDepthBuffer;
uniform sampler2D uniSceneColorBuffer;

in MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  vec3 vertNormals;
} fragMeshInfo;

void main() {
  vec3 norm = normalize(fragMeshInfo.vertNormals);
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

  vec3 viewDir = normalize(-fragMeshInfo.vertPosition);
  vec3 reflectDir = reflect(viewDir, norm);

  vec3 reflectColor = texture(uniSceneColorBuffer, reflectDir.xy).rgb; // Raymarching + binary search required

  gl_FragColor = vec4(reflectColor + (lightHitAngle * texture(uniTexture, fragMeshInfo.vertTexcoords).rgb), 1.0);
}
