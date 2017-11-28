#version 330 core

#define MAX_LIGHT_COUNT 10

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 vertTexcoords;
layout (location = 2) in vec3 vertNormals;

struct Light {
  vec4 position;
  vec3 direction;
  vec3 color;
  float angle;
};

uniform uint uniLightCount;
uniform Light uniLights[MAX_LIGHT_COUNT];
uniform mat4 uniMvpMatrix;

out vec2 fragTexcoords;
out float fragLightHitAngle;

void main() {
  gl_Position = vec4(vertPosition.xyz, 1.0) * uniMvpMatrix;
  fragTexcoords = vertTexcoords;

  if (uniLightCount > 0u) {
    for (uint lightIndex = 0u; lightIndex < uniLightCount; ++lightIndex)
      fragLightHitAngle /*+*/= clamp(dot(vertNormals, uniLights[lightIndex].direction), 0, 1);
  } else {
    fragLightHitAngle = 1;
  }
}
