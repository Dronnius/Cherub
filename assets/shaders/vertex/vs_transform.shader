#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 modelMatrix = mat4(1.0f);

void main()
{
	//vec4 intermediate = position;
	//intermediate.w = 1.0f;		//is this needed?
	gl_Position = modelMatrix * position;
};