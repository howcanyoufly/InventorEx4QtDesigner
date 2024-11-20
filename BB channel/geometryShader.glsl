#version 130
#extension GL_EXT_geometry_shader4 : enable

//!oiv_include <Inventor/oivShaderState.h>

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

uniform vec4 param1;
uniform vec4 param2;

uniform vec2 ab1;
uniform vec2 ab2;

uniform int width;
uniform int height;

out vec4 vColor;

float r(float angle, vec4 param, vec2 ab)
{
	float a = pow(abs(cos(param.x * angle / 4.0)/ab.x), param.z);
	float b = pow(abs(sin(param.x * angle / 4.0)/ab.y), param.w);
	
	return 1.0 / pow(a+b, 1.0 / param.y);
}

vec4 computePos(float theta, float phi, vec4 param1, vec4 param2, vec2 ab1, vec2 ab2)
{
  float r1 = r(theta, param1, ab1);
  float r2 = r(phi, param2, ab2);

  vec4 pos;
  pos.x = r1 * cos(theta) * r2 * cos(phi);
  pos.y = r1 * sin(theta) * r2 * cos(phi);
  pos.z = r2 * sin(phi);
  pos.w = 1.0;
  return pos;
}


void main()
{
  float xStep = 2.0 * 3.141592 / float(width-1);
  float yStep = 3.141592 / float(height);

  for(int i = 0; i < gl_VerticesIn; ++i) 
  {
	int id = int(gl_PositionIn[i].x);
	int xId = int(mod(id, width));
	int yId = int(id / width);

	float a = xId * xStep - 3.141592;
	float b = yId * yStep - 3.141592 * 0.5;
	
	vec4 v1 = computePos(a, b, param1, param2, ab1, ab2);
	vec4 v2 = computePos(a+xStep, b, param1, param2, ab1, ab2);
	vec4 v3 = computePos(a+xStep, b+yStep, param1, param2, ab1, ab2);
	vec4 v4 = computePos(a, b+yStep, param1, param2, ab1, ab2);
	
	vec3 n1 = abs(normalize(cross((v1 - v4).xyz, (v2 - v4).xyz)));
	vec3 n2 = abs(normalize(cross((v3 - v4).xyz, (v2 - v4).xyz)));
	
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v1;
	vColor = vec4(n1, 1.0);
	EmitVertex();
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v2;
	vColor = vec4(n1, 1.0);
	EmitVertex();
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v4;
	vColor = vec4(n1, 1.0);
	EmitVertex();
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v4;
	vColor = vec4(n2, 1.0);
	EmitVertex();
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v3;
	vColor = vec4(n2, 1.0);
	EmitVertex();
	gl_Position = OivProjectionMatrix() * OivModelViewMatrix() * v2;
	vColor = vec4(n2, 1.0);
	EmitVertex();
  }
}
