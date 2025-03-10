varying vec3 ecPosition3;
varying vec3 fragmentNormal;
void DirectionalLight(in int i,
                      in vec3 normal,
                      inout vec4 ambient,
                      inout vec4 diffuse,
                      inout vec4 specular)
{
  float nDotVP; // normal . light direction
  float nDotHV; // normal . light half vector
  float pf;     // power factor
  nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));
  nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));
  if (nDotVP == 0.0)
    pf = 0.0;
  else
    pf = pow(nDotHV, gl_FrontMaterial.shininess);
  ambient += gl_LightSource[i].ambient;
  diffuse += gl_LightSource[i].diffuse * nDotVP;
  specular += gl_LightSource[i].specular * pf;
}
void main(void)
{
  vec3 eye = -normalize(ecPosition3);
  vec4 ambient = vec4(0.0);
  vec4 diffuse = vec4(0.0);
  vec4 specular = vec4(0.0);
  vec3 color;
  DirectionalLight(0, normalize(fragmentNormal), ambient, diffuse, specular);
  color =
    gl_FrontLightModelProduct.sceneColor.rgb +
    ambient.rgb * gl_FrontMaterial.ambient.rgb +
    diffuse.rgb * gl_Color.rgb +
    specular.rgb * gl_FrontMaterial.specular.rgb;
  gl_FragColor = vec4(color, gl_Color.a);
}