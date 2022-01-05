#version 430 core
layout(std140) uniform;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
out vec2 TexCoords;

uniform MVPBlock{
	mat4 model;
	mat4 view;
	mat4 projection;
};

void main()
{
    gl_Position = projection  * vec4(aPos, 0.0, 1.0);
    TexCoords = aTexCoords;
}  