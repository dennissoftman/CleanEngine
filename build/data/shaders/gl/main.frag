#version 450 core

out vec4 fragColor;

uniform sampler2D img;

in vec2 texPos;

void main()
{
    fragColor = texture(img, texPos);
//    fragColor = vec4(texPos, 0, 1.0);
}
