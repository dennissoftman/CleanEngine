#version 450

layout(set = 0, binding = 1) uniform sampler2D img;

layout(location = 0) in vec2 texPos;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = texture(img, texPos);
}
