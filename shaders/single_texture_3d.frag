struct Material {
  sampler3D baseColorMap;
};

in struct MeshInfo {
  vec3 vertPosition;
  vec2 vertTexcoords;
  mat3 vertTBNMatrix;
} vertMeshInfo;

uniform Material uniMaterial;
uniform float uniDepth;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(texture(uniMaterial.baseColorMap, vec3(vertMeshInfo.vertTexcoords, uniDepth)).rgb, 1.0);
}
