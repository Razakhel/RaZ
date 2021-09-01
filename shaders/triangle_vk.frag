#version 450

layout(location = 0) in MeshInfo {
  vec2 vertTexcoords;
  vec3 vertColor;
} fragMeshInfo;

layout(binding = 1) uniform sampler2D uniTexture;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(uniTexture, fragMeshInfo.vertTexcoords);
}
