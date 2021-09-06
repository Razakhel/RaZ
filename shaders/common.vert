#version 330 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertTexcoords;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in ivec4 vertBoneIndices;
layout(location = 5) in vec4 vertBoneWeights;

uniform mat4 uniModelMatrix;
uniform mat4 uniMvpMatrix;

out MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} fragMeshInfo;

void main() {
  fragMeshInfo.vertPosition  = (uniModelMatrix * vec4(vertPosition, 1.0)).xyz;
  fragMeshInfo.vertTexcoords = vertTexcoords;

  mat3 modelMat = mat3(uniModelMatrix);

  vec3 tangent   = normalize(modelMat * vertTangent);
  vec3 normal    = normalize(modelMat * vertNormal);
  vec3 bitangent = cross(normal, tangent);
  fragMeshInfo.vertTBNMatrix = mat3(tangent, bitangent, normal);

  gl_Position = uniMvpMatrix * vec4(vertPosition, 1.0);
}
