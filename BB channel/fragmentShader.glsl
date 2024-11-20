#version 130

//!oiv_include <Inventor/oivShaderState.h>
//!oiv_include <Inventor/oivDepthPeeling_frag.h>

in vec4 vColor;

void main()
{
  OivDepthPeelingOutputColor(vColor);
}
