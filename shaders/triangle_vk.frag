#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in MeshInfo {
  vec2 vertTexcoords;
  vec3 vertColor;
} fragMeshInfo;

layout(binding = 1) uniform sampler2D uniTexture;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(uniTexture, fragMeshInfo.vertTexcoords);
}
