#version 450

layout(location = 0) in vec3 inPosition3D;
layout(location = 1) in vec3 inColor0;

layout(location = 2) in vec2 inPosition2D;
layout(location = 3) in vec3 inColor1;

layout(location = 0) out vec3 fragColor;

void main() {
    //gl_Position = vec4(inPosition2D, 0.0 , 1.0);
    gl_Position = vec4(inPosition3D, 1.0);
    fragColor = inColor1;
}