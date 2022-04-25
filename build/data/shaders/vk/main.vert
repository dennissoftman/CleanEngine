#version 450

layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec3 color;

layout(binding = 0) uniform VkShaderTransformData {
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;
} ubo;

void main()
{
    gl_Position = ubo.projMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(vertCoord, 1.0);
    
    color = vec3(texCoord, 0);
} 
