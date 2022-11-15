// Enabling the extension (if supported) on the shader's side is required if GL <4.0
#extension GL_ARB_tessellation_shader : enable

layout(triangles, equal_spacing, ccw) in;

void main() {
  gl_Position = vec4(0.0);
}
