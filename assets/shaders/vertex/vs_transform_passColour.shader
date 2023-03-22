#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colourIn;

out vec4 colourPass;

uniform mat4 modelMatrix = mat4(1.0f);

void main()
{
	//vec4 intermediate = position;
	//intermediate.w = 1.0f;		//is this needed?
	gl_Position = modelMatrix * position;
	//vec4 interColour = vec4(colourIn.r, colourIn.g, colourIn.b, 1.0f);
	colourPass = colourIn;
};