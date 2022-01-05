#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D textquad;

void main()
{
    vec3 col = texture(textquad, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
   // FragColor = vec4(vec3(1.0 - texture(frbTexture, TexCoords)), 1.0);
} 