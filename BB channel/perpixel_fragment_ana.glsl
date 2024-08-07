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

    vec3 lightDir = normalize(vec3(gl_LightSource[i].position));
    nDotVP = dot(normal, lightDir);
    nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));
  // 判断是否接近平行
  if (abs(nDotVP) < 0.01) { // 设定一个小的阈值，例如0.01，用于判断是否足够接近0
      nDotVP = 0.1; // 如果平行，给予一定的光照强度
      pf = 0.0; // 避免在平行面上产生高光
  } else {
      nDotVP = max(0.0, nDotVP); // 保持正常的光照计算，避免背面照光
      if (nDotVP > 0.0) {
          pf = pow(nDotHV, gl_FrontMaterial.shininess);
      } else {
          pf = 0.0;
      }
  }
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