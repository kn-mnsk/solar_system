#version 430 core

//layout(std140) uniform;

uniform vec4 color;

in flat float face;

out vec4 outColor;

void main()
{
	if (face == 2.0) {
	// Front face
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
		outColor = color;
	}
}
