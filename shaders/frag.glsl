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
  vec3 vertNormals;
} fragMeshInfo;

void main() {
  vec3 norm = normalize(fragMeshInfo.vertNormals);

  vec3 color = texture(uniTexture, fragMeshInfo.vertTexcoords).rgb;

  vec3 ambient = color * 0.05;
  vec3 diffuse = vec3(0.0);
  vec3 specular = vec3(0.0);

  for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex) {
    vec3 lightPos = (uniViewProjMatrix * uniLights[lightIndex].position).xyz;

    // Diffuse
    vec3 lightDir;

    if (uniLights[lightIndex].position.w == 0.0) {
      lightDir = normalize(lightPos - fragMeshInfo.vertPosition);
    } else {
      lightDir = uniLights[lightIndex].direction;
    }

    diffuse += max(dot(lightDir, norm), 0.0) * color;

    // Specular
    vec3 viewDir = normalize(-fragMeshInfo.vertPosition);
    vec3 halfDir = normalize(lightDir + viewDir);
    specular += uniLights[lightIndex].color * pow(max(dot(norm, halfDir), 0.0), 32.0);
  }

  gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}
