#version 430 core

//layout(std140) uniform;

in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D surface;


void main() {
	
	// get object color
    vec4 color = texture(surface, TexCoord);

//    //HDR tone mapping
//    float exposure = 1.0;
//    color  = vec4(vec3(1.0) - exp(-vec3(color) * exposure), 1.0);
//
    // ambient
    vec4 ambient = vec4(vec3(0.01), 1.0);
    
    // composite color
    vec3 result = vec3(color + ambient); 

    //HDR tone mapping
    float exposure = 1.5;
    result  = vec3(1.0) - exp(-result * exposure);

    //gamma correction
     const float gamma = 2.2;
     result = pow(result, vec3(1.0 / gamma));

     outColor = vec4(result, 1.0);
}