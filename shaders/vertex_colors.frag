in struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  vec3 vertColor;
  mat3 vertTBNMatrix;
} vertMeshInfo;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;

void main() {
  fragColor  = vec4(vertMeshInfo.vertColor, 1.0);
  fragNormal = vertMeshInfo.vertTBNMatrix[2];
}
