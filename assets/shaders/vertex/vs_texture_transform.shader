#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 texCoordPass;

uniform mat4 modelMatrix = mat4(1.0f);

void main()
{
	gl_Position = modelMatrix * position;
	texCoordPass = texCoord;
};