#version 430 core
layout(std140) uniform;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

out vec4 outColor;

uniform MVPBlock{
	mat4 model;
	mat4 view;
	mat4 projection;
};


void main() {
	outColor = aColor;
	gl_Position =  projection * view * model * vec4(aPos, 1.0);
   
}
