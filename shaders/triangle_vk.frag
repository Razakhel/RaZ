#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in MeshInfo {
  vec3 vertColor;
} fragMeshInfo;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(fragMeshInfo.vertColor, 1.0);
}
