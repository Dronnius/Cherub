#version 330 core

layout(location = 0) out vec4 colour;

in vec2 texCoordPass;

uniform sampler2D textureSampler;

void main()
{
	colour = texture(textureSampler, texCoordPass);
};