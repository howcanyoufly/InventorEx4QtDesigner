// 现代GLSL顶点着色器
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;
varying vec3 ecPosition3;
varying vec3 fragmentNormal;

void main(void) {
  vec4 ecPosition = u_modelViewMatrix * gl_Vertex;
  ecPosition3 = ecPosition.xyz / ecPosition.w;
  fragmentNormal = normalize(u_normalMatrix * gl_Normal);
  gl_Position = gl_ProjectionMatrix * ecPosition;
  gl_FrontColor = gl_Color;
}