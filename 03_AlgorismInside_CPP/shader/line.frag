#version 430 core

//layout(std140) uniform;


in vec4 outColor;

out vec4 fragColor;

//uniform vec4 objectColor;


void main()
{
	fragColor = outColor;
}
