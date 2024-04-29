#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main()
{    
    vec3 normal = texture(normalMap, fs_in.texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); // 将法线贴图的颜色从[0,1]转换到[-1,1]
    
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    
    vec3 color = diff * texture(diffuseMap, fs_in.texCoords).rgb;
    FragColor = vec4(color, 1.0);
}
