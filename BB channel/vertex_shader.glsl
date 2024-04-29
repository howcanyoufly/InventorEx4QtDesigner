#version 120 // GLSL版本，根据需要进行调整

// 输入
attribute vec4 a_position; // 顶点位置
attribute vec3 a_normal;   // 顶点法线

// 输出
varying vec3 v_normal;     // 传递给片元着色器的法线
varying vec3 v_position;   // 传递给片元着色器的位置

// Uniforms
uniform mat4 u_modelViewMatrix;  // 模型视图矩阵
uniform mat3 u_normalMatrix;     // 法线矩阵

void main() {
    v_position = vec3(u_modelViewMatrix * a_position); // 将位置转换到视图空间
    v_normal = normalize(u_normalMatrix * a_normal);   // 将法线转换并归一化
    
    gl_Position = u_modelViewMatrix * a_position; // 设置最终顶点位置
}
