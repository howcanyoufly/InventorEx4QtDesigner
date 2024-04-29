#version 330 core

out vec4 FragColor;

uniform vec4 highlightColor; // 高亮颜色，例如黄色

// GLSL没有直接的方式来修改深度值，但我们可以使用gl_FragDepth
void main() {
    FragColor = highlightColor; // 设置片元颜色为高亮颜色

    // 微调深度值，使其略微“靠前”
    // 注意：这个值的调整需要根据你的场景和深度缓冲区的具体配置来微调
    gl_FragDepth = gl_FragCoord.z - 0.0001;
}
