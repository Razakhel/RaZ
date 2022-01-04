layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertTexcoords;
layout(location = 2) in vec3 vertNormal;
layout(location = 3) in vec3 vertTangent;
layout(location = 4) in mat4 instMatrix;

layout(std140) uniform uboCameraInfo {
  mat4 uniViewMat;
  mat4 uniInvViewMat;
  mat4 uniProjectionMat;
  mat4 uniInvProjectionMat;
  mat4 uniViewProjectionMat;
  vec3 uniCameraPos;
};

layout(std140) uniform uboModelInfo {
  mat4 uniModelMat;
};

out struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} vertMeshInfo;

void main() {
  mat4 modelMat = instMatrix * uniModelMat;

  vertMeshInfo.vertPosition  = (modelMat * vec4(vertPosition, 1.0)).xyz;
  vertMeshInfo.vertTexcoords = vertTexcoords;

  mat3 modelMat3 = mat3(modelMat);

  vec3 tangent   = normalize(modelMat3 * vertTangent);
  vec3 normal    = normalize(modelMat3 * vertNormal);
  vec3 bitangent = cross(normal, tangent);
  vertMeshInfo.vertTBNMatrix = mat3(tangent, bitangent, normal);

  gl_Position = uniViewProjectionMat * modelMat * vec4(vertPosition, 1.0);
}
