#version 130
#extension GL_EXT_gpu_shader4 : enable

//!oiv_include <Inventor/oivShapeAttribute.h>
//!oiv_include <Inventor/oivShaderState.h>

void main()
{
  gl_Position = vec4(gl_VertexID+0.5, 0.0, 0.0, 1.0);
}
