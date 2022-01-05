#version 430 core

//layout(std140) uniform;

uniform vec4 color;

out vec4 outColor;

void main()
{
	outColor = color;
}
