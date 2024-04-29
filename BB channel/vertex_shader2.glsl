#version 330 core

// 输入
layout(location = 0) in vec4 vertexPosition; // 顶点位置，假定绑定到位置0
layout(location = 1) in vec3 vertexNormal;   // 顶点法线，假定绑定到位置1
layout(location = 2) in vec4 vertexColor;    // 顶点颜色，假定绑定到位置2

// 输出到片元着色器
out vec3 ecPosition3;
out vec3 fragmentNormal;
out vec4 fragmentColor;

// Uniforms
uniform mat4 modelViewMatrix;  // 模型视图矩阵
uniform mat3 normalMatrix;     // 法线矩阵

void main() {
    // 将顶点位置转换到眼睛坐标系
    vec4 ecPosition = modelViewMatrix * vertexPosition;
    ecPosition3 = ecPosition.xyz / ecPosition.w;
    
    // 将法线转换到眼睛坐标系并归一化
    fragmentNormal = normalize(normalMatrix * vertexNormal);
    
    // 传递顶点颜色到片元着色器
    fragmentColor = vertexColor;
    
    // 设置最终顶点位置
    gl_Position = modelViewMatrix * vertexPosition;
}
