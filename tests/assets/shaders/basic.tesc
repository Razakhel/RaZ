// Enabling the extension (if supported) on the shader's side is required if GL <4.0
#extension GL_ARB_tessellation_shader : enable

layout(vertices = 3) out;

void main() {
  gl_TessLevelOuter[0] = 1.0;
  gl_TessLevelOuter[1] = 2.0;
  gl_TessLevelOuter[2] = 3.0;
  gl_TessLevelOuter[3] = 4.0;

  gl_TessLevelInner[0] = 1.0;
  gl_TessLevelInner[1] = 2.0;
}
