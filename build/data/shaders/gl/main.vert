#version 330 core
layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec3 vertexColor;

out vec3 color;

void main()
{
    color = vertexColor;

    gl_Position.xy = vertexPos;
    gl_Position.z = 0;
    gl_Position.w = 1;
}
