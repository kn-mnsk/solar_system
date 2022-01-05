#ifndef MYCURSOR_HPP
#define MYCUESOR_HPP// custom cursor creation

#include <glad/glad.h>
#include <GLFW/glfw3.h>

unsigned char pixels[16 * 16 * 4];
memset(pixels, 0xff, sizeof(pixels));
GLFWimage image;
image.width = 16;
image.height = 16;

/*unsigned char color[4] = { 255, 0, 0, 1 };
for (int i = 0; i < 16 * 16; i += 4)
{
    pixels[i] =  0x80;
    pixels[i + 1] = 0x80;
    pixels[i + 2] = 0x00;
    pixels[i + 3] = 0xFF;
}

unsigned char c = pixels[1];
for (int i = 0; i < 8; ++i) {
    bool is_set = c & (1 << i);
    std::cout << "Bit " << i << ": " << is_set << '\n';
}

*/
image.pixels = pixels;

GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
glfwSetCursor(window, cursor);
// end of custom cursor creation#pragma once


#endif