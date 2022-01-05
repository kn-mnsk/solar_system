#version 430 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D text;
uniform vec4 fontcolor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    fragColor = fontcolor * sampled;
}  