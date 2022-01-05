#ifndef TEXT_H
#define TEXT_H

#include <iostream>
#include <map>
#include <string>


#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <learnopengl/custom/myshader.h>
#include <learnopengl/original/filesystem.h>

namespace TextRendering {

	class Text {
	private:
		// configure VAO/VBO for texture quads
		GLuint VAO, VBO;

		struct Character {
			unsigned int TextureID; // ID handle of the glyph texture
			glm::ivec2   Size;      // Size of glyph
			glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
			unsigned int Advance;   // Horizontal offset to advance to next glyph
		};
		Character character;

	public:
		std::map<GLchar, Character> Characters;

		std::string FilePath{};
		std::string TextString{};
		float X{ 0.0f };
		float Y{ 0.0f };
		glm::vec2 Scale{ 1.0f };
		glm::vec4 FontColor{ 0.0f, 0.0f, 0.0f, 1.0f };


		Text(std::string fontname) {

			//FilePath = FileSystem::getPath(fontname);
			FilePath = fontname;
			
			bool success = loadFont(FilePath);
			if (!success) {

				std::cout << "ERROR::Text class: Failed to generate instance" << std::endl;
				throw std::runtime_error("error: failed!\n");
			}

		}


		void Draw(GLuint shaderID, std::string text, float x, float y, glm::vec2 scale, glm::vec4 fontcolor)
		{
			TextString = text;
			X = x;
			Y = y;
			Scale = scale;
			FontColor = fontcolor;
			GLsizei stride{ 4 * sizeof(float) };
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			// activate corresponding render state	
			glUseProgram(shaderID);
			glUniform4fv(glGetUniformLocation(shaderID, "fontcolor"), 1, glm::value_ptr(FontColor));
			glActiveTexture(GL_TEXTURE0);  // default unit no is used  "text" texture
			glBindVertexArray(VAO);

			// iterate through all characters
			std::string::const_iterator c;
			for (c = TextString.begin(); c != TextString.end(); c++)
			{
				Character ch = Characters[*c];

				float xpos = X + ch.Bearing.x * Scale.x;
				float ypos = Y - (ch.Size.y - ch.Bearing.y) * Scale.y;

				float w = ch.Size.x * Scale.x;
				float h = ch.Size.y * Scale.y;
				// update VBO for each character
				float vertices[6][4] = {
					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos,     ypos,       0.0f, 1.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },

					{ xpos,     ypos + h,   0.0f, 0.0f },
					{ xpos + w, ypos,       1.0f, 1.0f },
					{ xpos + w, ypos + h,   1.0f, 0.0f }
				};
				// render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch.TextureID);
				// update content of VBO memory
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// render quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				X += (ch.Advance >> 6) * Scale.x; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
			}

			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);

		}


	private:
		FT_Library ft;
		FT_Face face;

		// Calculates the front vector from the Camera's (updated) Euler Angles
		bool loadFont(std::string filepath)
		{
			if (FT_Init_FreeType(&ft))
			{
				std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
				return false;
			}

			if (FT_New_Face(ft, filepath.c_str(), 0, &face)) {
				std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
				return false;
			}


			// set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 48);

			// disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glBindTexture(GL_TEXTURE_2D, 0);

				// now store character for later use
				character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				Characters.insert(std::pair<char, Character>(c, character));
			}
			//glBindTexture(GL_TEXTURE_2D, 0);


			// destroy FreeType once we're finished
			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			return true;
		}

	};

}

// TEXT_H
#endif