#version 450 core
layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 texPos;

void main()
{
    texPos = texCoord;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertCoord, 1.0);
}
