#version 330 core

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = position;
	gl_Position.w = 1.0;		//is this needed?
};