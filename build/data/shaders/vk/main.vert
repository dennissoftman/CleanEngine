#version 450

layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec3 vertexColor;

layout(location = 0) out vec3 color;

layout(push_constant) uniform constants
{
    mat4 modelMatrix;
} obj;

void main()
{
    color = vertexColor;
    
    gl_Position = obj.modelMatrix * vec4(vertexPos + vec2(gl_InstanceIndex%10, gl_InstanceIndex/10)*0.01, 0.0, 1.0);
} 
