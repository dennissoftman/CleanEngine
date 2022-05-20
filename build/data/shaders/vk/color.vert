#version 450
layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normCoord;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform TransformData
{
	mat4 projection;
	mat4 view;
} sceneData;

layout(push_constant) uniform ObjectData
{
	mat4 model;
	vec4 color;
} objectData;

void main()
{
	color = objectData.color;
	gl_Position = sceneData.projection * sceneData.view * objectData.model * vec4(vertCoord, 1.0);
}
