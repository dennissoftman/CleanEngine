#version 330 core
layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec3 vertexColor;

uniform mat4 modelMatrix;

out vec3 color;

void main()
{
    color = vertexColor;

    gl_Position = modelMatrix * vec4(vertexPos, 0.0, 1.0);
}
