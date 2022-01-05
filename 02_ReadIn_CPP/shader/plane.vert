#version 430 core
layout(std140) uniform;

layout(location = 0) in vec3 aPos;

uniform MVPBlock{
	mat4 model;
	mat4 view;
	mat4 projection;
};


void main() {

	gl_Position =  projection * view * model * vec4(aPos, 1.0);

}