#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 1.0, 0.0, 1.0); // 假设你想保持原有的颜色输出不变

    // 微调深度值，使其略微“靠前”
    gl_FragDepth = gl_FragCoord.z - 0.0001;
}
