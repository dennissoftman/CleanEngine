#version 450
layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normCoord;

layout(location = 0) out vec2 texPos;

layout(set = 0, binding = 0) uniform TransformData
{
	mat4 projection;
	mat4 view;
	mat4 model;
} objectData;

void main()
{
	texPos = texCoord;
	gl_Position = objectData.projection * objectData.view * objectData.model * vec4(vertCoord, 1.0);
}
