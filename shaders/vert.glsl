#version 330 core

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 vertTexcoords;
layout (location = 2) in vec3 vertNormal;

uniform mat4 uniModelMatrix;
uniform mat4 uniMvpMatrix;

out MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  vec3 vertNormal;
} fragMeshInfo;

out vec3 fragNormal;

void main() {
  vec4 pos = uniMvpMatrix * vec4(vertPosition.xyz, 1.0);
  vec3 norm = mat3(uniModelMatrix) * vertNormal;

  fragMeshInfo.vertPosition = pos.xyz;
  fragMeshInfo.vertTexcoords = vertTexcoords;
  fragMeshInfo.vertNormal = norm;

  fragNormal = norm;

  gl_Position = pos;
}
