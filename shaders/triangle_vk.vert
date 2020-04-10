#version 450

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;

layout(binding = 0) uniform Matrices {
  mat4 model;
  mat4 view;
  mat4 proj;
} uniMatrices;

layout(location = 0) out MeshInfo {
  vec3 vertColor;
} fragMeshInfo;

void main() {
  gl_Position            = uniMatrices.proj * uniMatrices.view * uniMatrices.model * vec4(vertPosition, 1.0);
  fragMeshInfo.vertColor = vertColor;
}
