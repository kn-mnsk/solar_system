#version 430 core
//layout(std140) uniform;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;


out VERT_OUT {
    vec3 ViewPos;
    vec3 ViewNormal;
    vec2 TexCoord;
	mat4 View;
} vert_out;

uniform	mat4 model;
layout(std140) uniform ViewBlock{
	mat4 view;
};
layout(std140) uniform	ProjectionBlock{
	mat4 projection;
};


void main() {
	vec4 pos = view * model * vec4(aPos, 1.0);
	vert_out.ViewPos = vec3(pos);

	gl_Position =  projection * pos;

	mat4 Mat4 = transpose(inverse(view * model));
	vert_out.ViewNormal = vec3(normalize(Mat4* vec4(aNormal, 0.0)));	
	vert_out.TexCoord = aTexCoord;
	vert_out.View = view;

}