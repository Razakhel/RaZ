#version 450

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertColor;

layout(location = 0) out MeshInfo {
  vec3 vertColor;
} fragMeshInfo;

void main() {
  gl_Position            = vec4(vertPosition, 1.0);
  fragMeshInfo.vertColor = vertColor;
}
