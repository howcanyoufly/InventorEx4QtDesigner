void main(void) {
    // 将顶点变换到眼睛（视图）空间
    vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

    // 模拟polygonOffset：在眼睛空间沿z轴移动顶点
    ecPosition.z += 1.0;  // 假设眼睛空间中向相机移动

    // 应用投影变换
    gl_Position = gl_ProjectionMatrix * ecPosition;

    // 将顶点颜色传递到光栅化阶段
    gl_FrontColor = gl_Color;
}
