#ifndef MYTOOLS2_H
#define  MYTOOLS2_H

#include <iostream>
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <glimg/glimg.h>
#include <glimg/ImageCreatorExceptions.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/custom/mycamera.h>

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

const float PI = 3.14159265f;

// screen settings
int SCR_WIDTH = 1300;
int SCR_HEIGHT = 975;

float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// camera
//glm::vec3 camPos(0.0f, 5.0f, 10.0f);
//MyCamera::Camera camera = MyCamera::Camera{ camPos };

struct Vertex2 {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};


struct meshVertices {
	std::vector<Vertex2> vertices;
	std::vector<GLuint> indices;
};

struct LineVertex {
	glm::vec3 pos;
	glm::vec4 color;
	//glm::vec3 endpos;
	//glm::vec4 endcolor;
};

struct LineVertices {
	std::vector<LineVertex> segments; // to element
	std::vector<GLuint> indices; // two element
};


// Function declalation
void Console_Fontsize(int a, int b);
void showOpenGLInfo(GLenum name);
std::vector<std::string> Get_Program_Infos();
std::string GetCurrentDir();
HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
void callback_FramebufferSize(GLFWwindow* window, int width, int height);
void callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods);
void callback_CursorPos(GLFWwindow* window, double xpos, double ypos);
void callback_MouseButton(GLFWwindow* window, int button, int action, int mods);
void callback_Scroll(GLFWwindow* window, double xoffset, double yoffset);
void callback_WindowSize(GLFWwindow* window, int width, int height);
void display();
GLuint loadTexture(const char* path, bool flip_vertically);
GLuint loadTexture_DDS(char const* path);
GLuint loadTexture_HDR(char const* path, bool flip_vertically);
GLuint loadTexture_NonHDR(char const* path, bool flip_vertically);
GLuint loadArrayTexture_NonHDR(std::vector<std::string> path, bool flip_vertically);
GLuint loadArrayTexture_NonHDR();
void genMeshSphere(GLuint x_segs, GLuint y_segs, meshVertices& trianglestrip, meshVertices& patches, float sphereRadius = 1.0f);
void genMeshTorus(GLfloat R, GLuint R_segs, GLfloat r, GLuint r_segs, meshVertices& trianglestrip, meshVertices& patches);
void genMeshParaboloid(GLfloat semimajor, GLfloat semiminor, GLuint elliptic_segs, GLfloat height, GLuint height_segs, meshVertices& trianglestrip, meshVertices& patches);
void genMeshCube(meshVertices& patches, LineVertices& normalline, float bendnormal = 0.0f);
void genMeshQuad(meshVertices& patches);
void genMeshLine(LineVertices& line);
void genMeshAxes(LineVertices& axes);
void drawElements(GLuint vao, GLuint vbo, GLuint ebo, GLenum mode, GLuint indicessize, const void* indices);
void bindBufferTriangleStrip(meshVertices& trianglestrip, GLuint& vao, GLuint& vbo, GLuint& ebo);
void bindBufferPatches(meshVertices& patches, GLuint& vao, GLuint& vbo, GLuint& ebo);
void bindBufferLines(LineVertices& lines, GLuint& vao, GLuint& vbo, GLuint& ebo);
void renderCube(GLuint vao, GLuint vbo);
void renderQuad(GLuint vao, GLuint vbo);
glm::mat3 genTBNMat3(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec3 nm);


void Console_Fontsize(int a, int b) {
	PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx = new CONSOLE_FONT_INFOEX();
	lpConsoleCurrentFontEx->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
	lpConsoleCurrentFontEx->dwFontSize.X = a;
	lpConsoleCurrentFontEx->dwFontSize.Y = b;
	SetCurrentConsoleFontEx(out, 0, lpConsoleCurrentFontEx);
}

void showOpenGLInfo(GLenum name)
{
	const GLubyte* gl_string = glGetString(name);

	std::cout << "OPENGL Info: " << gl_string << "\n" << std::endl;

}


std::vector<std::string> Get_Program_Infos()
{

	std::vector<std::string> program_infos;

	char* pValue = _pgmptr;
	//printf("The full path of the executing program is : %s\n", pValue);

	std::string sValue = (std::string)pValue;
	size_t size = sValue.length();

	size_t currentP = -1;
	size_t lastP;
	do {
		currentP += 1;
		lastP = currentP - 1;
		currentP = sValue.find("\\", currentP);
		//std::cout << "last /" << currentP << " " << lastP << std::endl;

	} while (currentP < size);

	std::string name = sValue.substr(lastP + 1, size - 1);
	std::string execDir = sValue.substr(0, lastP - 1);
	size = name.length();
	name = name.erase(size - 4);
	//std::cout << "last /" << lastP << " " << name << std::endl;
	program_infos.push_back(name);
	program_infos.push_back(execDir);

	char buff[FILENAME_MAX]; //create string buffer to hold path
	char* wk_ch = _getcwd(buff, FILENAME_MAX);
	std::string cwd(buff);
	program_infos.push_back(cwd);


	std::cout << "Program Name: " << program_infos.at(0) << "\n";
	std::cout << "Exec Directory: " << program_infos.at(1) << "\n";
	std::cout << "Current Working Dir: " << program_infos.at(2) << "\n" << std::endl;;

	return program_infos;
}


std::string GetCurrentDir() {
	char buff[FILENAME_MAX]; //create string buffer to hold path
	char* wk_ch = _getcwd(buff, FILENAME_MAX);
	std::string cwd(buff);
	std::cout << "Current Working Directory: " << cwd << std::endl;

	return cwd;
}

void debugMat4(std::string name, glm::mat4 matrix) {

	std::cout << name + ": " << std::endl;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			std::cout << matrix[column][row] << ", " << std::ends;
		}
		std::cout << std::endl;
	}
	std::cout << "\n" << std::endl;

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	SCR_WIDTH = width;
//	SCR_HEIGHT = height;
//	glViewport(0, 0, width, height);
//}
//
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//
//	// need to implement in case of use
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window,  double xoffset, double yoffset)
//{
//	// need to implement in case of use
//}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------

void genMeshSphere(GLuint x_segs, GLuint y_segs, meshVertices& trianglestrip, meshVertices& patches, float sphereRadius) {
	// Generate sphere vertices for GL_TRIANGLE_STRIP

	std::vector<Vertex2> vertices{};
	std::vector<GLuint> indices{};
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	//std::vector<GLuint> indices;

	for (unsigned int y = 0; y <= y_segs; ++y)
	{
		for (unsigned int x = 0; x <= x_segs; ++x)
		{
			float xSegment = (float)x / (float)x_segs;
			float ySegment = (float)y / (float)y_segs;
			float theta = ySegment * PI; float phi = xSegment * 2.0f * PI;
			float xPos = sphereRadius * sinf(theta) * sinf(phi);
			float yPos = sphereRadius * cosf(theta);
			float zPos = sphereRadius * sinf(theta) * cosf(phi);

			position = glm::vec3(xPos, yPos, zPos);
			normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
			texcoord = glm::vec2(xSegment, ySegment);

			vertices.push_back(Vertex2{ position, normal, texcoord });
		}
	}

	bool oddMeridian;
	//int k;
	for (int x = 0; x <= x_segs; ++x) {
		oddMeridian = x % 2;
		int k = (x + 1) % (x_segs + 1);
		if (!oddMeridian) {
			for (int y = 0; y <= y_segs; ++y) {
				indices.push_back(x + y * (y_segs + 1));
				indices.push_back(k + y * (y_segs + 1));
			}
		}
		else {
			for (int y = y_segs; y >= 0; --y) {
				indices.push_back(x + y * (y_segs + 1));
				indices.push_back(k + y * (y_segs + 1));
			}
		}
	}

	trianglestrip.vertices.clear();
	trianglestrip.indices.clear();
	trianglestrip = meshVertices{ vertices, indices };

	{ // output vertices and indices of trianglestrips
		std::ofstream trianglesIndices_file;
		trianglesIndices_file.open("../../../data/other/trianglesIdices.txt");
		for (unsigned int i = 0; i < trianglestrip.indices.size(); i++) {
			trianglesIndices_file << trianglestrip.indices[i] << "\n";
		}
		trianglesIndices_file.close();

		std::ofstream trianglesPos_file;
		trianglesPos_file.open("../../../data/other/trianglesPosition.txt");
		for (unsigned int i = 0; i < trianglestrip.vertices.size(); i++) {
			trianglesPos_file << trianglestrip.vertices[i].position.x << "," << trianglestrip.vertices[i].position.y << "," << trianglestrip.vertices[i].position.z << "\n";
		}
		trianglesPos_file.close();
	}


	// Tesselation - GL_PATCHES
	vertices.clear();
	indices.clear();
	int k, k_;
	for (int i = 0; i < trianglestrip.indices.size() - 2; i++) {
		bool oddIndex = i % 2;
		for (int j = 0; j < 3; j++) {
			k = (int)trianglestrip.indices[i + j]; // cw
			k_ = (int)trianglestrip.indices[(i + 2) - j]; // reverse ccw
			// let winding order be same
			if (!oddIndex) { // cw
				position = trianglestrip.vertices[k].position;
				normal = trianglestrip.vertices[k].normal;
				texcoord = trianglestrip.vertices[k].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}
			else {// cw from ccw: reverse winding order in case of even index
				position = trianglestrip.vertices[k_].position;
				normal = trianglestrip.vertices[k_].normal;
				texcoord = trianglestrip.vertices[k_].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}

			indices.push_back(3 * i + j);
		}
	}

	patches.vertices.clear();
	patches.indices.clear();
	patches = meshVertices{ vertices, indices };

	{ // output vertices and indices of patches
		std::ofstream patchesIndices_file;
		patchesIndices_file.open("../../../data/other/patchesIdices.txt");
		for (unsigned int i = 0; i < patches.indices.size(); i++) {
			patchesIndices_file << patches.indices[i] << "\n";
		}
		patchesIndices_file.close();

		std::ofstream patchesPos_file;
		patchesPos_file.open("../../../data/other/patchesPosition.txt");
		for (unsigned int i = 0; i < patches.vertices.size(); i++) {
			patchesPos_file << patches.vertices[i].position.x << "," << patches.vertices[i].position.y << "," << patches.vertices[i].position.z << "\n";
		}
		patchesPos_file.close();
	}


}


void genMeshTorus(GLfloat R, GLuint R_segs, GLfloat r, GLuint r_segs, meshVertices& trianglestrip, meshVertices& patches) {
	// Generate vertices for GL_TRIANGLE_STRIP

	std::vector<Vertex2> vertices{};
	std::vector<GLuint> indices{};
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	for (unsigned int j = 0; j <= R_segs; ++j)
	{
		for (unsigned int k = 0; k <= r_segs; ++k)
		{
			float RSegment = (float)j / (float)R_segs;
			float rSegment = (float)k / (float)r_segs;
			float theta = rSegment * 2.0f * PI;
			float phi = RSegment * 2.0f * PI;
			float xPos = (R + r * cosf(theta)) * sinf(phi);
			float yPos = r * sinf(theta);
			float zPos = (R + r * cos(theta)) * cosf(phi);

			position = glm::vec3(xPos, yPos, zPos);
			normal = position - R * glm::vec3(sinf(phi), 0.0f, cosf(phi));
			normal = glm::normalize(normal);
			texcoord = glm::vec2(RSegment, rSegment);

			vertices.push_back(Vertex2{ position, normal, texcoord });
		}
	}

	//bool oddMeridian;
	//int k;
	for (unsigned int k = 0; k <= r_segs; ++k) {
		//oddMeridian = j % 2;
		int m = (k + 1) % (r_segs + 1);
		//if (!oddMeridian) {
		for (unsigned int j = 0; j <= R_segs; ++j) {
			indices.push_back(k + j * (r_segs + 1));
			indices.push_back(m + j * (r_segs + 1));
		}
		/*}
		else {
			for (int k = r_segs; k >= 0; --k) {
				indices.push_back(j + j * (r_segs + 1));
				indices.push_back(m + k * (r_segs + 1));
			}
		}*/
	}

	trianglestrip.vertices.clear();
	trianglestrip.indices.clear();
	trianglestrip = meshVertices{ vertices, indices };

	//{ // output vertices and indices of trianglestrips
	//	std::ofstream trianglesIndices_file;
	//	trianglesIndices_file.open("trianglesIdices.txt");
	//	for (int i = 0; i < trianglestrip.indices.size(); i++) {
	//		trianglesIndices_file << trianglestrip.indices[i] << "\n";
	//	}
	//	trianglesIndices_file.close();

	//	std::ofstream trianglesPos_file;
	//	trianglesPos_file.open("trianglesPosition.txt");
	//	for (int i = 0; i < trianglestrip.vertices.size(); i++) {
	//		trianglesPos_file << trianglestrip.vertices[i].position.x << "," << trianglestrip.vertices[i].position.y << "," << trianglestrip.vertices[i].position.z << "\n";
	//	}
	//	trianglesPos_file.close();
	//}


	// Tesselation - GL_PATCHES
	vertices.clear();
	indices.clear();
	GLuint k, k_;
	for (unsigned int i = 0; i < trianglestrip.indices.size() - 2; i++) {
		bool oddIndex = i % 2;
		for (unsigned int j = 0; j < 3; j++) {
			k = trianglestrip.indices[i + j]; // cw
			k_ = trianglestrip.indices[i + 2 - j]; // reverse ccw
			// let winding order be same
			if (!oddIndex) { // cw
				position = trianglestrip.vertices[k].position;
				normal = trianglestrip.vertices[k].normal;
				texcoord = trianglestrip.vertices[k].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}
			else {// cw from ccw: reverse winding order in case of even index
				position = trianglestrip.vertices[k_].position;
				normal = trianglestrip.vertices[k_].normal;
				texcoord = trianglestrip.vertices[k_].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}

			indices.push_back(3 * i + j);
		}
	}

	patches.vertices.clear();
	patches.indices.clear();
	patches = meshVertices{ vertices, indices };

	//{ // output vertices and indices of patches
	//	std::ofstream patchesIndices_file;
	//	patchesIndices_file.open("patchesIdices.txt");
	//	for (int i = 0; i < patches.indices.size(); i++) {
	//		patchesIndices_file << patches.indices[i] << "\n";
	//	}
	//	patchesIndices_file.close();

	//	std::ofstream patchesPos_file;
	//	patchesPos_file.open("patchesPosition.txt");
	//	for (int i = 0; i < patches.vertices.size(); i++) {
	//		patchesPos_file << patches.vertices[i].position.x << "," << patches.vertices[i].position.y << "," << patches.vertices[i].position.z << "\n";
	//	}
	//	patchesPos_file.close();
	//}


}


void genMeshParaboloid(GLfloat semimajor, GLfloat semiminor, GLuint elliptic_segs, GLfloat height, GLuint height_segs, meshVertices& trianglestrip, meshVertices& patches) {
	// Generate vertices for GL_TRIANGLE_STRIP

	std::vector<Vertex2> vertices{};
	std::vector<GLuint> indices{};
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	for (unsigned int j = 0; j <= height_segs; ++j)
	{
		for (unsigned int k = 0; k <= elliptic_segs; ++k)
		{

			float heightSeg = (float)j / (float)height_segs;
			float ellipticSeg = (float)k / (float)elliptic_segs;
			float u = heightSeg * height; // from 0.0 to height
			float v = ellipticSeg * 2.0f * PI; // from 0.0 to 2PI
			float xPos = semimajor * u * cosf(v);
			float yPos = glm::pow(u, 2.0f);
			float zPos = semiminor * u * sinf(v);

			position = glm::vec3(xPos, yPos, zPos);
			// nomal
			float c1 = glm::pow((2.0f * u * cosf(v) * semiminor), 2.0f) + glm::pow((2.0f * u * sinf(v) * semimajor), 2.0f) + glm::pow(semimajor * semiminor, 2.0f);
			float c2 = glm::pow(semimajor * semiminor, 2.0f) + 2.0f * glm::pow(u, 2.0f) * cosf(2.0f * v) * (-glm::pow(semimajor, 2.0f) + glm::pow(semiminor, 2.0f)) + 2.0f * glm::pow(u, 2.0f) * (glm::pow(semimajor, 2.0f) + glm::pow(semiminor, 2.0f));

			normal.x = 2.0f * u * cosf(v) * semiminor / glm::sqrt(c1);
			normal.y = -semimajor * semiminor / glm::sqrt(c2);
			normal.z = 2.0f * u * sinf(v) * semimajor / glm::sqrt(c1);
			normal = glm::normalize(normal);

			texcoord = glm::vec2(heightSeg, ellipticSeg);

			vertices.push_back(Vertex2{ position, normal, texcoord });
		}
	}

	bool oddMeridian;
	//int k;
	for (int k = 0; k <= elliptic_segs; ++k) {
		oddMeridian = k % 2;
		int m = (k + 1) % (elliptic_segs + 1);
		if (!oddMeridian) {
			for (int j = 0; j <= height_segs; ++j) {
				indices.push_back(k + j * (elliptic_segs + 1));
				indices.push_back(m + j * (elliptic_segs + 1));
			}
		}
		else {
			for (int j = height_segs; j >= 0; --j) {
				indices.push_back(k + j * (elliptic_segs + 1));
				indices.push_back(m + j * (elliptic_segs + 1));
			}
		}
	}

	trianglestrip.vertices.clear();
	trianglestrip.indices.clear();
	trianglestrip = meshVertices{ vertices, indices };

	{ // output vertices and indices of trianglestrips
		std::ofstream trianglesIndices_file;
		trianglesIndices_file.open("trianglesIdices.txt");
		for (unsigned int i = 0; i < trianglestrip.indices.size(); i++) {
			trianglesIndices_file << trianglestrip.indices[i] << "\n";
		}
		trianglesIndices_file.close();

		std::ofstream trianglesPos_file;
		trianglesPos_file.open("trianglesPosition.txt");
		for (unsigned int i = 0; i < trianglestrip.vertices.size(); i++) {
			trianglesPos_file << trianglestrip.vertices[i].position.x << "," << trianglestrip.vertices[i].position.y << "," << trianglestrip.vertices[i].position.z << "\n";
		}
		trianglesPos_file.close();
	}


	// Tesselation - GL_PATCHES
	vertices.clear();
	indices.clear();
	GLuint k, k_;
	for (unsigned int i = 0; i < trianglestrip.indices.size() - 2; i++) {
		bool oddIndex = i % 2;
		for (unsigned int j = 0; j < 3; j++) {
			k = trianglestrip.indices[i + j]; // cw
			k_ = trianglestrip.indices[i + 2 - j]; // reverse ccw
			// let winding order be same
			if (!oddIndex) { // cw
				position = trianglestrip.vertices[k].position;
				normal = trianglestrip.vertices[k].normal;
				texcoord = trianglestrip.vertices[k].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}
			else {// cw from ccw: reverse winding order in case of even index
				position = trianglestrip.vertices[k_].position;
				normal = trianglestrip.vertices[k_].normal;
				texcoord = trianglestrip.vertices[k_].texcoord;
				vertices.push_back(Vertex2{ position, normal, texcoord });
			}

			indices.push_back(3 * i + j);
		}
	}

	patches.vertices.clear();
	patches.indices.clear();
	patches = meshVertices{ vertices, indices };

	//{ // output vertices and indices of patches
	//	std::ofstream patchesIndices_file;
	//	patchesIndices_file.open("patchesIdices.txt");
	//	for (int i = 0; i < patches.indices.size(); i++) {
	//		patchesIndices_file << patches.indices[i] << "\n";
	//	}
	//	patchesIndices_file.close();

	//	std::ofstream patchesPos_file;
	//	patchesPos_file.open("patchesPosition.txt");
	//	for (int i = 0; i < patches.vertices.size(); i++) {
	//		patchesPos_file << patches.vertices[i].position.x << "," << patches.vertices[i].position.y << "," << patches.vertices[i].position.z << "\n";
	//	}
	//	patchesPos_file.close();
	//}


}

// Generate cube 1x1 3D cube, as patches, no trianglestrip.
void genMeshCube(meshVertices& patches, LineVertices& normalline, float bendnormal)
{ // bendnormal in degree

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;

	//trianglestrip.vertices.clear();
	//trianglestrip.indices.clear();
	//position = glm::vec3(0.0f, 0.0f, 0.0f);
	//normal = glm::vec3(0.0f, 0.0f, 0.0f);
	//texcoord = glm::vec2(0.0f, 0.0f);
	//trianglestrip.vertices.push_back(Vertex2{ position, normal, texcoord });
	//trianglestrip.indices.push_back(0);


	std::vector<Vertex2> vertices{};

	std::vector<Vertex2> VNT{ // back face only of cube
		// bottom-left
		Vertex2{glm::vec3(0.5f, -0.5f, -0.5f),  // vertex
					glm::vec3(0.0f,  0.0f, -1.0f), // normal
					glm::vec2(0.0f, 0.0f)}, // texcord
		 // top-left
		Vertex2{glm::vec3(0.5f,  0.5f, -0.5f),
					glm::vec3(0.0f,  0.0f, -1.0f),
					glm::vec2(0.0f, 1.0f)},
					// top-right
					Vertex2{glm::vec3(-0.5f,  0.5f, -0.5f),
								glm::vec3(0.0f,  0.0f, -1.0f),
								glm::vec2(1.0f, 1.0f)},
								// bottom-right
								Vertex2{glm::vec3(-0.5f, -0.5f, -0.5f),
											glm::vec3(0.0f,  0.0f, -1.0f),
											glm::vec2(1.0f, 0.0f)},
											//////////////////////////////////////////////////
											// // top-left
											//Vertex2{glm::vec3(1.0f,  1.0f, -1.0f),
											//			glm::vec3(0.0f,  0.0f, -1.0f),
											//			glm::vec2(0.0f, 1.0f)},
											//// bottom-left
											//Vertex2{glm::vec3(1.0f, -1.0f, -1.0f),  // vertex
											//			glm::vec3(0.0f,  0.0f, -1.0f), // normal
											//			glm::vec2(0.0f, 0.0f)}, // texcord
											//// bottom-right
											//Vertex2{glm::vec3(-1.0f, -1.0f, -1.0f),
											//			glm::vec3(0.0f,  0.0f, -1.0f),
											//			glm::vec2(1.0f, 0.0f)},
											//// top-right
											//Vertex2{glm::vec3(-1.0f,  1.0f, -1.0f),
											//			glm::vec3(0.0f,  0.0f, -1.0f),
											//			glm::vec2(1.0f, 1.0f)},
	};

	// orient normal -> manupulation of tessellation
	for (int i = 0; i < VNT.size(); i++) {
		glm::mat4 rotAxisX{ 1.0f };
		rotAxisX = glm::rotate(rotAxisX, glm::radians(-45.0f - 90.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 newX = rotAxisX * glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		glm::mat4 rot{ 1.0f };
		//bendnormal = glm::pow(-1.0f, i) * bendnormal;
		rot = glm::rotate(rot, glm::radians(bendnormal), newX);
		normal = glm::vec3(rot * glm::vec4(VNT[i].normal, 0.0f));
		VNT[i].normal = normal;
	}


	//Four faces; back face, right face, front face and left face, toward +Z axis
	for (int i = 0; i <= 3; i++) {
		glm::mat4 rotY{ 1.0f };
		rotY = glm::rotate(rotY, glm::radians(90.0f * i), glm::vec3(0.0f, 1.0f, 0.0f));
		for (int j = 0; j < VNT.size(); j++) {
			position = glm::vec3(rotY * glm::vec4(VNT[j].position, 1.0f));
			normal = glm::vec3(rotY * glm::vec4(VNT[j].normal, 0.0f));
			texcoord = VNT[j].texcoord;
			vertices.push_back(Vertex2{ position, normal, texcoord });
		}
	}

	//Top face and Bootm face
	for (int i = 1; i <= 3; i += 2) {
		glm::mat4 rotX{ 1.0f };
		rotX = glm::rotate(rotX, glm::radians(90.0f * i), glm::vec3(1.0f, 0.0f, 0.0f));
		for (int j = 0; j < VNT.size(); j++) {
			position = glm::vec3(rotX * glm::vec4(VNT[j].position, 1.0f));
			normal = glm::vec3(rotX * glm::vec4(VNT[j].normal, 0.0f));
			texcoord = VNT[j].texcoord;
			vertices.push_back(Vertex2{ position, normal, texcoord });
		}
	}

	std::vector<GLuint> indices{};

	for (int i = 0; i < vertices.size(); i += 4) {
		for (int j = 0; j <= 1; j++) {
			indices.push_back(i);
			indices.push_back(i + 1 + j);
			indices.push_back(i + 2 + j);
			//indices.push_back(i + 2 + j);
			//indices.push_back(i + 1 + j);
		}
	}

	patches.vertices.clear();
	patches.indices.clear();
	patches = meshVertices{ vertices, indices };

	//{ // output vertices and indices of patches
	//	std::ofstream patchesIndices_file;
	//	patchesIndices_file.open("patchesIdices.txt");
	//	for (int i = 0; i < patches.indices.size(); i++) {
	//		patchesIndices_file << patches.indices[i] << "\n";
	//	}
	//	patchesIndices_file.close();

	//	std::ofstream patchesPos_file;
	//	patchesPos_file.open("patchesPosition.txt");
	//	for (int i = 0; i < patches.vertices.size(); i++) {
	//		patchesPos_file << patches.vertices[i].position.x << "," << patches.vertices[i].position.y << "," << patches.vertices[i].position.z << "\n";
	//	}
	//	patchesPos_file.close();
	//}

	// normal line
	normalline.segments.clear();
	normalline.indices.clear();
	glm::vec4 whitecolor{ 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < vertices.size(); i++) {
		normalline.segments.push_back(LineVertex{ vertices[i].position, whitecolor });
		normalline.segments.push_back(LineVertex{ (vertices[i].position + vertices[i].normal), whitecolor});
		normalline.indices.push_back(2 * i);
		normalline.indices.push_back(2 * i + 1);

	}

}

void genMeshQuad(meshVertices& patches) {

	//generate patches

	patches.vertices = {
		// positions, normal, texture Coords
		Vertex2{glm::vec3(-1.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),			glm::vec2(0.0f, 1.0f)},
		Vertex2{glm::vec3(1.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f),			glm::vec2(1.0f, 1.0f)},
		Vertex2{glm::vec3(1.0f,  0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f),			glm::vec2(1.0f, 0.0f)},
		Vertex2{glm::vec3(-1.0f,  0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f),			glm::vec2(0.0f, 0.0f)},
	};
	// setup plane VAO
	patches.indices = {
		0, 1, 3,
		1, 2, 3,
	};

}

void genMeshLine(LineVertices& line) {

	// normal line
	line.segments.clear();
	line.indices.clear();

	glm::vec4 whitecolor{0.0f, 0.0f, 0.0f, 1.0f}; // initial color
	line.segments.push_back( LineVertex{glm::vec3(0.0f, 0.0f, 0.0f), whitecolor} );
	line.segments.push_back( LineVertex{glm::vec3(0.0f, 1.0f, 0.0f), whitecolor });
	line.indices = { 0, 1 };

}


void genMeshAxes(LineVertices& axes) {

	// normal line
	axes.segments.clear();
	axes.indices.clear();
	float length = 0.50f;
	// initial color
	glm::vec4 red{1.0f, 0.0f, 0.0f, 1.0f}; // X axis
	glm::vec4 green{ 0.0f, 1.0f, 0.0f, 1.0f }; // Y axis
	glm::vec4 blue{ 0.0f, 0.0f, 1.0f, 1.0f }; // Z axis
	axes.segments.push_back(LineVertex{ glm::vec3(-length, 0.0f, 0.0f), red });
	axes.segments.push_back(LineVertex{ glm::vec3(length, 0.0f, 0.0f), red });
	axes.segments.push_back(LineVertex{ glm::vec3(0.0f, -length, 0.0f), green });
	axes.segments.push_back(LineVertex{ glm::vec3(0.0f, length, 0.0f), green });
	axes.segments.push_back(LineVertex{ glm::vec3(0.0f, 0.0f, -length), blue });
	axes.segments.push_back(LineVertex{ glm::vec3(0.0f, 0.0f, length), blue });
	axes.indices = { 0, 1, 2, 3, 4, 5 };

}


void bindBufferTriangleStrip(meshVertices& trianglestrip, GLuint& vao, GLuint& vbo, GLuint& ebo) {

	std::vector<Vertex2> vertices{};
	std::vector<GLuint> indices{};

	vertices = trianglestrip.vertices;
	indices = trianglestrip.indices;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void bindBufferPatches(meshVertices& patches, GLuint& vao, GLuint& vbo, GLuint& ebo) {

	bindBufferTriangleStrip(patches, vao, vbo, ebo);

	/*std::vector<Vertex2> vertices{};
	std::vector<GLuint> indices{};


	vertices = patches.vertices;
	indices = patches.indices;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}



void bindBufferLines(LineVertices& lines, GLuint& vao, GLuint& vbo, GLuint& ebo) {

	std::vector<LineVertex> vertices{};
	std::vector<GLuint> indices{};

	vertices = lines.segments;
	indices = lines.indices;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void drawElements(GLuint vao, GLuint vbo, GLuint ebo, GLenum mode, GLuint indicessize, const void* indices) {

	GLsizei stride{};

	switch (mode) {
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_PATCHES: {
		stride = sizeof(Vertex2);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

		glDrawElements(mode, indicessize, GL_UNSIGNED_INT, 0);// no need for indices as argument!!

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
		break;
	}

	case GL_LINES: {
		stride = sizeof(LineVertex);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

		glDrawElements(mode, indicessize, GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);
		break;
	}
	}

}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void renderQuad(GLuint vao, GLuint vbo)
{
	if (vao == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// utility function for loading a 2D texture from file
GLuint loadTexture(char const* path, bool flip_vertically) {

	GLuint textureID{ 0 };
	

	// first, check dds file
	std::string pathStr = (std::string)path;
	std::size_t dds_found = pathStr.find(".dds");
	if (dds_found == std::string::npos) {
		dds_found = pathStr.find(".DDS");
	}

	if (dds_found != std::string::npos) {
		textureID = loadTexture_DDS(path);
		return textureID;
	}

	// then, png, jpeg, files
	bool hdr = stbi_is_hdr(path);


	if (hdr)
	{
		textureID = loadTexture_HDR(path, flip_vertically);
	}
	else
	{
		textureID = loadTexture_NonHDR(path, flip_vertically);
	}


	return textureID;
}

GLuint loadTexture_HDR(char const* path, bool flip_vertically) {
	GLuint textureID{ 0 };

	GLsizei width{ 0 };
	GLsizei height{ 0 };
	GLsizei channels{ 0 };
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(flip_vertically);
	float* data = stbi_loadf(path, &width, &height, &channels, 0);

	if (!data) {
		std::cout << "\nHDR Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return textureID;
	}

	// data is not empy!
	GLint internalformat{ GL_RGB16F };
	GLenum format{ GL_RGB };
	GLenum type{ GL_FLOAT };

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	stbi_image_free(data);

	return textureID;
}



GLuint loadTexture_DDS(char const* path) {
	GLuint textureID{ 0 };

	std::auto_ptr<glimg::ImageSet> pImageSet;

	try
	{
		pImageSet.reset(glimg::loaders::dds::LoadFromFile(path));
		glimg::SingleImage imagedata = pImageSet->GetImage(0, 0, 0);

		glGenTextures(1, &textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (imagedata.GetDimensions()).width, (imagedata.GetDimensions()).height, 0, GL_RED, GL_UNSIGNED_BYTE, imagedata.GetImageData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());
		//throw;
	}


	return textureID;
}



GLuint loadTexture_NonHDR(char const* path, bool flip_vertically) {
	GLuint textureID{ 0 };

	GLsizei width{ 0 };
	GLsizei height{ 0 };
	GLsizei channels{ 0 };
	std::vector<unsigned char> texture{};
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(flip_vertically);
	unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

	if (!data) {
		std::cout << "Non-HDR Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return textureID;
	}

	/*
	int pixelSize = width * height *channels;
	for (int j = 0; j < pixelSize; j++) {
		texture.push_back(data[j]);
	}*/

	//// reverse the picture
	//for (int h = height - 1; h >= 0; h--) {
	//	for (int w = 0; w < width; w++) {
	//		for (int c = 0; c < channels; c++) {
	//			texture.push_back(data[h * width * channels + w * channels + c]);
	//		}
	//	}
	//}

	//stbi_image_free(data);

	// data is not empy!
	GLint internalformat{ GL_RGB8 };
	GLenum format{ GL_RGB };
	GLenum type{ GL_UNSIGNED_BYTE };

	switch (channels)
	{
	case 1:
	{
		internalformat = GL_RED;
		format = GL_RED;
		/*internalformat = GL_RGB8;
		format = GL_RGB;*/
		break;
	}
	case 3:
	{
		internalformat = GL_RGB8;
		format = GL_RGB;
		break;
	}
	case 4:
	{
		internalformat = GL_RGBA8;
		format = GL_RGBA;
		break;
	}
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);// &texture[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//texture.clear();
	stbi_image_free(data);

	return textureID;
}


GLuint loadArrayTexture_NonHDR(std::vector<std::string> path, bool flip_vertically) {
	GLuint textureID{ 0 };

	GLsizei width{ 0 };
	GLsizei height{ 0 };
	GLsizei channels{ 0 };
	GLsizei layerCount = (GLsizei)path.size();
	GLsizei mipLevelCount{ 1 };

	std::vector<unsigned char> arrayTexture{};

	for (int i = 0; i < layerCount; i++) {
		// tell stb_image.h to flip loaded texture's on the y-axis.
		stbi_set_flip_vertically_on_load(flip_vertically);
		unsigned char* data = stbi_load(path[i].c_str(), &width, &height, &channels, 0);
		if (!data) {
			std::cout << "\nOne of ArrayTexture failed to load at path: " << path[i] << std::endl;
			stbi_image_free(data);
			continue;
			//}/*
			//int pixelSize = width * height *channels;
			//for (int j = 0; j < pixelSize; j++) {
			//	arrayTexture.push_back(data[j]);
			//}*/
			//// reverse the picture
			//for (int h = height - 1; h >= 0; h--) {
			//	for (int w = 0; w < width; w++) {
			//		for (int c = 0; c < channels; c++) {
			//			arrayTexture.push_back(data[h * width*channels + w*channels + c]);
			//		}
			//	}
		}

		stbi_image_free(data);
	}

	if (arrayTexture.empty()) {
		std::cout << "\nAll of ArrayTexture failed to load " << std::endl;
		return textureID;
	}

	// arrayTexture is not empy, but may be a fewer layer count as expected!
	GLint internalformat{ GL_RGB8 };
	GLenum format{ GL_RGB };
	GLenum type{ GL_UNSIGNED_BYTE };

	switch (channels)
	{
	case 1:
	{
		internalformat = GL_RED;
		format = GL_RED;
		break;
	}
	case 3:
	{
		internalformat = GL_RGB8;
		format = GL_RGB;
		break;
	}
	case 4:
	{
		internalformat = GL_RGBA8;
		format = GL_RGBA;
		break;
	}
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, internalformat, width, height, layerCount);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, format, type, &arrayTexture[0]);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	arrayTexture.clear();

	return textureID;
}

GLuint loadArrayTexture_NonHDR() {// ArrayTexture Sample coding
	GLuint textureID = 0;

	GLsizei width = 2;
	GLsizei height = 2;
	GLsizei layerCount = 6;
	GLsizei mipLevelCount = 1;

	// Read you texels here. In the current example, we have 2*2*6 = 24 texels, with each texel being 4 GLubytes.
	//GLubyte texels[96] =
	//unsigned char texels[96] =
	std::vector<unsigned char> texels = {
		// Texels for first image.
		0,   0,   0,   255,
		255, 0,   0,   255,
		0,   255, 0,   255,
		0,   0,   255, 255,
		// Texels for second image.
		255, 255, 255, 255,
		255, 255,   0, 255,
		0,   255, 255, 255,
		255, 0,   255, 255,
		// Texels for thirdd image.
		255, 255, 255, 255,
		255, 255,   0, 255,
		255,   255, 255, 255,
		255, 0,   255, 255,
		// Texels for 4th image.
		255, 255, 255, 255,
		255, 255,   0, 255,
		255,   255, 255, 255,
		255, 0,   255, 255,
		// Texels for 5th image.
		255, 255, 255, 255,
		255, 255,   0, 255,
		255,   255, 255, 255,
		255, 0,   255, 255,
		// Texels for 6th image.
		255, 255, 255, 255,
		255, 255,   0, 255,
		255,   255, 255, 255,
		255, 0,   255, 255,
	};

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
	// Allocate the storage.
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
	// Upload pixel data.
	// The first 0 refers to the mipmap level (level 0, since there's only 1)
	// The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
	// The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
	// Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.
	//glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, texels);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, &texels[0]);

	// Always set reasonable texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;

}



glm::vec3 genTangentVec3(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
{
	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 T; // tangent
	//glm::vec3 B; // bitangent;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	T.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	T.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	T.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	T = glm::normalize(T);

	//B.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	//B.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	//B.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	//B = glm::normalize(B);

	return T;
}


#endif