// Solar SYSTEM
// https://www.solarsystemscope.com/ 

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE  // not known well

#include <glimg/glimg.h>
#include <glimg/ImageCreatorExceptions.h>

#include <iostream>

// always to be written at last
#include <learnopengl/original/filesystem.h>

#include <learnopengl/custom/myshader.h>
#include <learnopengl/custom/MyTimer.h>
#include <learnopengl/custom/mycamera.h>

#include <configuration/root_directory.h>

#include <mydevelop/mytools2.h>
#include <mydevelop/text.h>
#include <mydevelop/solartool2.h>

GLFWwindow* window;
//SolarSys::Stars* stars;

bool clickFirst{ true };
float timeAtClickFirst{ 0.0f };
bool clickSecond{ false };
float timeAtClickSecond{ 0.0f };
float diff;

const float depthZNear = 0.0f;
const float depthZFar = 1.0f;

float nearClipPlane{};
float farClipPlane{};

// timing
float deltaTime{ 0.0f };
float lastFrame{ 0.0f };

float solarTime{ 0.0f };

meshVertices  cubeMeshPatches{};
meshVertices  sphereMeshTriangleStrip{}, sphereMeshPatches{};
meshVertices  quadMeshPatches{};
LineVertices cubeNormalLine{};
LineVertices lineMesh{};
LineVertices axesMesh{};

enum BuffType {
	QuadBuff,
	CubeBuff,
	SphereBuff,
	LineBuff,
	AxesBuff,

	Num_BuffType,
};

//Buffers
GLuint VAOs[Num_BuffType]{ 0 }, VBOs[Num_BuffType]{ 0 }, EBOs[Num_BuffType]{ 0 };

// Frame buffer
GLuint FBO{ 0 }, RBO{ 0 }; // frame buffer, render buffer
GLuint textquad{ 0 }; // color attachment texture
float quadX = -0.30f;
float quadY = 0.98f;
float dW = 0.5f;
float dH = 0.4f;
//glm::vec2 topLeft(quadX, quadY);
std::vector<float> quadVertices;
std::vector<float> calcQuadVertices(float qx, float qy, float dw, float dh) { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
	// positions   // texCoords
	std::vector<float> vertices{ qx,  qy,  0.0f, 1.0f,
	qx,  qy - dh,  0.0f, 0.0f,
	qx + dw,  qy - dh,  1.0f, 0.0f,

	qx,  qy,  0.0f, 1.0f,
	qx + dw,  qy - dh,  1.0f, 0.0f,
	qx + dw,  qy,  1.0f, 1.0f };
	return vertices;
}

// screen quad VAO
unsigned int quadVAO, quadVBO;
TextRendering::Text* textRenderer;

// viewport parameters vector
std::vector<std::vector<int>> vParami;
std::vector<std::vector<float>> vParamf;

float cursorPosXf{ 0.0f };
float cursorPosYf{ 0.0f };

enum CameraID
{
	SolarCamera,
	EarthCamera,
	MoonCamera,

	Num_CameraID,
};

float radius{}; // in pixel
float theta{}; //in degree
float phi{}; // in degree;

glm::vec3 calcEyePosition(float radius, float theta, float phi) {
	glm::vec3 eye;
	eye.x = radius * sinf(glm::radians(theta)) * sinf(glm::radians(phi));
	eye.y = radius * cosf(glm::radians(theta));
	eye.z = radius * sinf(glm::radians(theta)) * cosf(glm::radians(phi));

	return eye;
}

glm::vec3 eye{};
glm::vec3 target{};
glm::vec3 up{};
MyCamera::Camera* cameraPtr{nullptr};



int keyToggle{ 0 };

const float PI2 = PI * 2.0f;
const float g_unitSphereRadiusPX{ 0.5f }; // in PX

const glm::vec3 ZeroV3{ 0.0f, 0.0f, 0.0f };
const glm::vec3 X_AxisV3{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AxisV3{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AxisV3{ 0.0f, 0.0f, 1.0f };

const glm::vec3 g_solarOrigin{ ZeroV3 };  // World Space
const glm::vec3 g_eclipticOrigin{ ZeroV3 };  // World Space = Ecliptic Space

const glm::vec3 g_originOfEclipticSys{ ZeroV3 };
const glm::vec3 g_originOfCelestrialSys{ ZeroV3 };

int g_displayWidthFirst = SCR_WIDTH;;  // even number
int g_displayWidth = SCR_WIDTH;  // even number
int g_displayHeightFirst = SCR_HEIGHT;  // even number
int g_displayHeight = SCR_HEIGHT;  // even number

int g_framebufferWidth = SCR_WIDTH;
int g_framebufferHeight = SCR_HEIGHT;

int g_viewportX;
int g_viewportWidth;
int g_viewportY;
int g_viewportHeight;
int g_viewportDelta{ 1 };

bool g_showInCelestial{ false };


MyTimer::Timer g_solarTimer = MyTimer::Timer(MyTimer::Timer::Type::TT_INFINITE);

enum TimerControl {
	Stop,
	Slower,
	Normal,
	Fast,
	Faster,

	Num_TimerControl
};

float g_timeScale;  //milliseconds per day
float timeThrough;
float g_daysPassed{ 0.0f };
CameraID Camera_in_use;
TimerControl g_timerControl;   // keep the previous control
CameraID g_screen; // keep the preious screen//


enum Switch {
	Off,
	On,

	Num_Switch
};

Switch g_writeSwitchCamera{ Off };
Switch g_writeSwitchMotion{ Off };
Switch g_showCameraPositions{ Off };



enum ProjectionType {
	Pers,
	PersFOV,
	InfinitePers,
	Ortho,
	PixelOrtho,

	Num_ProjectionType,
};

enum ViewPort4 {
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,

	Num_ViewPort4
};


ViewPort4 g_port[Num_CameraID] =
{ TopLeft, BottomLeft, BottomRight };


// Funtion Declaration
void keyboard_Timecontrol(GLFWwindow* window, int key, int scancode, int action, int mods);
void keyboard_CameraPosition(GLFWwindow* window, int key, int scancode, int action, int mods);
void keyboard_Standard(GLFWwindow* window, int key, int scancode, int action, int mods);
void processOneClick(GLFWwindow* window, int button, int action, int mods);
void processDoubleClick(GLFWwindow* window, int button, int action, int mods);
void projection(int w, int h, ProjectionType projectiontype = Pers);
void drawText(GLuint shaderprogram);
void callback_DrawStars(std::vector<CSVReader::MotionDataFormatCSV> datalist); 


void updateTimerControl(TimerControl control) {

	switch (control) {
	case Stop:
		if (!g_solarTimer.IsPaused())
			g_solarTimer.TogglePause();
		break;
	case Slower:
		if (g_solarTimer.IsPaused())
			g_solarTimer.TogglePause();
		//equivalent to  30 seconds one day
		break;
	case Normal:
		if (g_solarTimer.IsPaused())
			g_solarTimer.TogglePause();
		g_solarTimer.Fastforward(0.050f); // equivlent to 2 sec per day
		break;
	case Fast:
		if (g_solarTimer.IsPaused())
			g_solarTimer.TogglePause();
		g_solarTimer.Fastforward(0.5f); // 0.33 seconds per 
		break;
	case Faster:
		if (g_solarTimer.IsPaused())
			g_solarTimer.TogglePause();
		g_solarTimer.Fastforward(5.0f);// 0.1 sec per 
		break;
	}

}

const float g_fHalfLightDistance = 200.0f;
const float g_attenuCoefficient = 1.0f / glm::pow(g_fHalfLightDistance, 2.0f);

bool g_drawLightLines = true;

int g_currMaterial = 1;
const int NUM_MATERIALS = 2;

SolarSys::MaterialBlock getMaterial(int i) {
	std::vector<SolarSys::MaterialBlock> mtls(NUM_MATERIALS);

	SolarSys::MaterialBlock mtl;
	mtl.diffuseColor = glm::vec4(1.0f, 0.673f, 0.043f, 1.0f);
	mtl.specularColor = glm::vec4(1.0f, 0.673f, 0.043f, 1.0f);// *0.4f;
	mtl.specularShininess = 0.125f;
	mtls[0] = mtl;

	//mtl.diffuseColor = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
	//mtl.specularColor = glm::vec4(0.99f, 0.99f, 0.99f, 1.0f);
	//mtl.specularColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	//mtl.specularShininess = 0.125f;
	mtl.diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // sun light
	mtl.specularColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f); //glm::vec4(0.99f, 0.99f, 0.99f, 1.0f);
	mtl.specularShininess = 0.6f; // 1.0f;// 0.3f; // 0.125f;
	mtls[1] = mtl;
	if (i > NUM_MATERIALS) {
		mtl = {};
		return mtl;
	}

	return mtls[i];
}


bool first{ true };
void resetSolarSystem()
{


	g_solarTimer.Reset();
	//g_solarTimer.Update();
	timeThrough = g_solarTimer.GetTimeSinceStart();
	updateTimerControl(g_timerControl);


}


//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void initializing() {

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);
	//glEnable(GL_SCISSOR_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_GEQUAL);
	glDepthRange(depthZNear, depthZFar);
	glEnable(GL_DEPTH_CLAMP);


	// generate BUffers: vao, ebo, vbo
	glGenVertexArrays(Num_BuffType, &VAOs[0]);
	glGenBuffers(Num_BuffType, &VBOs[0]);
	glGenBuffers(Num_BuffType, &EBOs[0]);

	// Cube
	genMeshCube(cubeMeshPatches, cubeNormalLine, 0.0f);
	bindBufferPatches(cubeMeshPatches, VAOs[CubeBuff], VBOs[CubeBuff], EBOs[CubeBuff]);
	// Sphere: model sphere raidus = 0.5f
	genMeshSphere(32, 32, sphereMeshTriangleStrip, sphereMeshPatches, g_unitSphereRadiusPX);
	bindBufferTriangleStrip(sphereMeshTriangleStrip, VAOs[SphereBuff], VBOs[SphereBuff], EBOs[SphereBuff]);
	// Quad - Plane
	genMeshQuad(quadMeshPatches);
	bindBufferPatches(quadMeshPatches, VAOs[QuadBuff], VBOs[QuadBuff], EBOs[QuadBuff]);
	// Line
	genMeshLine(lineMesh);
	bindBufferLines(lineMesh, VAOs[LineBuff], VBOs[LineBuff], EBOs[LineBuff]);
	// Axes
	genMeshAxes(axesMesh);
	bindBufferLines(axesMesh, VAOs[AxesBuff], VBOs[AxesBuff], EBOs[AxesBuff]);

	// Text Rendering
	textRenderer = new TextRendering::Text{ get_globaldata_root("fonts/archivo-narrow.regular.ttf") };


	//std::string motionCSV{ "other/motiondataArchive/solarsys_motiondata_feynman_7300.csv" };
	std::string motionCSV{ get_globaldata_root("other/motiondataArchive/solarsys_motiondata_rungekutta2_73000.csv") };
	std::string constantCSV{ get_globaldata_root("other/solarsys_constantdata.csv") };
	std::string shaderCSV{ get_globaldata_root("other/solarsys_shaderdata.csv") };
	std::string textureCSV{ get_globaldata_root("other/solarsys_texturedata.csv") };

	SolarSys::initialSetup(motionCSV, constantCSV, shaderCSV, textureCSV, 14);


	// FrameBuffer Configuration for Text rendering
	// generate Frame buffer, color attchmen texture and render buffer
	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &textquad);
	glGenRenderbuffers(1, &RBO);

	// screen quad VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

}

void resetCamera() {

	//cursorPosXf = g_displayWidth / 2.0f;
	//cursorPosYf = g_displayHeight /2.0f;

	nearClipPlane = 1.0f;
	farClipPlane = 3000.0f;

	radius = 1300.0f; // in pixel
	theta = 75.0f; //in degree
	phi = 15.0f; // in degree;

	eye = calcEyePosition(radius, theta, phi);
	target = glm::vec3 { 0.0f, 0.0f, 0.0f };
	up = glm::vec3 { 0.0f, 1.0f, 0.0f };
	cameraPtr = new MyCamera::Camera{ eye, up };
	//cameraPtr->Front = glm::normalize(target - eye);

}


int main() {
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	std::vector<std::string> program_info = Get_Program_Infos();
	std::string title = "LeranOpenGL Exercise - " + program_info.at(0);
	window = glfwCreateWindow(g_displayWidth, g_displayHeight, title.c_str(), NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window\n" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// set callbacks
	glfwSetFramebufferSizeCallback(window, callback_FramebufferSize);
	glfwSetKeyCallback(window, callback_Key);
	glfwSetCursorPosCallback(window, callback_CursorPos);
	glfwSetMouseButtonCallback(window, callback_MouseButton);
	glfwSetScrollCallback(window, callback_Scroll);
	glfwSetWindowSizeCallback(window, callback_WindowSize);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);// questionable!!

	glfwSetWindowPos(window, 1980, 70);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Maximize window at the beginning
	glfwMaximizeWindow(window);

	glfwGetWindowSize(window, &g_displayWidthFirst, &g_displayHeightFirst);
	g_displayWidth = g_displayWidthFirst;
	g_displayHeight = g_displayHeightFirst;

	showOpenGLInfo(GL_VERSION);

	initializing();

	resetCamera();

	while (!glfwWindowShouldClose(window)) {

		// per-frame time logic
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		SolarSys::drawStars(callback_DrawStars);

	}

	glfwTerminate();
	// Exit program
	exit(EXIT_SUCCESS);
}

void callback_DrawStars(std::vector<CSVReader::MotionDataFormatCSV> datalist) {

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, g_displayWidth, g_displayHeight);

	// View 
	glm::mat4 viewMat{ cameraPtr->GetViewMatrix()};
	glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("MVPBlock").unifBuffObject);
	glBufferSubData(GL_UNIFORM_BUFFER, SolarSys::mvpSubData.at(SolarSys::subDataType::view).offset, SolarSys::mvpSubData.at(SolarSys::subDataType::view).size, glm::value_ptr(viewMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Projection
	projection(g_displayWidth, g_displayHeight);

	// Other UNiform Block if any
	std::map<std::string, SolarSys::UnifBlocks>::iterator it;
	// Material Block
	it = SolarSys::unifBlocks.find("MaterialBlock");
	if (it != SolarSys::unifBlocks.end()) {
		SolarSys::MaterialBlock mtlData = getMaterial(g_currMaterial);
		//bindingIndex = SolarSys::shader.at(shaderName).unifBlockBindingPoint[2];
		glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("MaterialBlock").unifBuffObject);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, SolarSys::unifBlocks.at("MaterialBlock").unifBlockSize, &mtlData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Light Block
	it = SolarSys::unifBlocks.find("LightBlock");
	if (it != SolarSys::unifBlocks.end()) {
		//if (SolarSys::shader.at(shaderName).unifBlockBindingPoint.size() == 4) {
		SolarSys::LightBlock lightData;
		lightData.ambientIntensity = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);//(0.01f, 0.01f, 0.01f, 1.0f)
		lightData.attenuCoefficient = g_attenuCoefficient;
		lightData.gamma = 2.2f;
		lightData.maxIntensity = 2.0f;
		//bindingIndex = SolarSys::shader.at(shaderName).unifBlockBindingPoint[3];
		glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("LightBlock").unifBuffObject);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, SolarSys::unifBlocks.at("LightBlock").unifBlockSize, &lightData);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Draw Axes
	// Model
	float size = 1100.0f;
	glm::mat4 modelMat{ 1.0f };
	modelMat = glm::scale(modelMat, glm::vec3(size));
	glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("MVPBlock").unifBuffObject);
	glBufferSubData(GL_UNIFORM_BUFFER, SolarSys::mvpSubData.at(SolarSys::subDataType::model).offset, SolarSys::mvpSubData.at(SolarSys::subDataType::model).size, glm::value_ptr(modelMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	GLuint axesProg = SolarSys::shader.at("axes").shaderPtr->ID;
	glUseProgram(axesProg);
	std::vector<GLuint> Indices = axesMesh.indices;
	//GLenum bind_Mode = LineBuff;
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0f);
	drawElements(VAOs[AxesBuff], VBOs[AxesBuff], EBOs[AxesBuff], GL_LINES, (GLuint)Indices.size(), &Indices[0]);
	glDisable(GL_LINE_SMOOTH);

	glUseProgram(0);


	// Draw stars
	int planetId;
	std::string shaderName;
	GLuint prog;
	for (CSVReader::MotionDataFormatCSV data : datalist) {
		solarTime = data.systimer; // note: same in the datalist, which consist of motion data with same systimer 
		planetId = data.planetId;
		shaderName = SolarSys::constant.at(planetId).shaderName;

		float size = SolarSys::constant.at(planetId).size * 1.0f;
		modelMat = glm::mat4{ 1.0f };
		glm::vec4 XYZ{ data.scaledPositionX, data.scaledPositionY, data.scaledPositionZ, 0.0f };
		modelMat = glm::translate(modelMat, glm::vec3(XYZ));// OPenGL coordinates
		modelMat = glm::scale(modelMat, glm::vec3(size));

		// Model
		glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("MVPBlock").unifBuffObject);
		glBufferSubData(GL_UNIFORM_BUFFER, SolarSys::mvpSubData.at(SolarSys::subDataType::model).offset, SolarSys::mvpSubData.at(SolarSys::subDataType::model).size, glm::value_ptr(modelMat));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		// program
		prog = SolarSys::shader.at(shaderName).shaderPtr->ID;
		glUseProgram(prog);

		// texture binding
		for (std::string texname : SolarSys::constant.at(planetId).texName) {

			GLuint texunitno = SolarSys::texture.at(texname).texUnitNo;
			GLuint texobject = SolarSys::texture.at(texname).texObject;
			glActiveTexture(GL_TEXTURE0 + texunitno);
			glBindTexture(GL_TEXTURE_2D, texobject);
		}

		GLenum gl_Mode = GL_TRIANGLE_STRIP; // or GL_PATCHES only
		std::vector<GLuint> Indices = sphereMeshTriangleStrip.indices;
		GLenum bind_Mode = SphereBuff;
		drawElements(VAOs[bind_Mode], VBOs[bind_Mode], EBOs[bind_Mode], gl_Mode, (GLuint)Indices.size(), &Indices[0]);

		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

	}

	// Draw text
	// projection should be orthogonal
	projection(g_displayWidth, g_displayHeight, Ortho);
	drawText(SolarSys::shader.at("text").shaderPtr->ID);


	glfwSwapBuffers(window);
	glfwPollEvents();

}

void drawText(GLuint shaderprogram) {

	// Text data  Note: Ascii charcters only
	const int numMsg{ 6 };
	const int bufSize{ 100 };
	char textbuffer[numMsg][bufSize];
	//snprintf(textbuffer[0], static_cast<size_t>(bufSize), "Quad Position: X=%#5.4f : %#4d, Y=%#5.4f : %#4d", quadX, g_displayWidth, quadY, g_displayHeight);
	std::string s = "============================";
	snprintf(textbuffer[0], static_cast<size_t>(bufSize), s.c_str());
	snprintf(textbuffer[1], static_cast<size_t>(bufSize), "Days passed: %#6.2f", solarTime);
	snprintf(textbuffer[2], static_cast<size_t>(bufSize), s.c_str());
	snprintf(textbuffer[3], static_cast<size_t>(bufSize), "Camera Position = ( %#7.2f, %#7.2f, %#7.2f)", cameraPtr->Position.x, cameraPtr->Position.y, cameraPtr->Position.z);
	snprintf(textbuffer[4], static_cast<size_t>(bufSize), "Camera Front = ( %#7.2f, %#7.2f, %#7.2f)", cameraPtr->Front.x, cameraPtr->Front.y, cameraPtr->Front.z);
	snprintf(textbuffer[5], static_cast<size_t>(bufSize), "Camera Fov = %#4.1f", cameraPtr->Fov);
	//snprintf(textbuffer[7], static_cast<size_t>(bufSize), "Eye Position: theta=%#6.2f, phi=%#6.2f", g_cameraPara[Camera_in_use][g_cameraConstants[Camera_in_use].star].camEyePos.theta, g_cameraPara[Camera_in_use][g_cameraConstants[Camera_in_use].star].camEyePos.phi);
	//snprintf(textbuffer[8], static_cast<size_t>(bufSize), "Euler Angle: Yaw=% #6.2f, Pitch=%#6.2f, Roll=%#6.2f", g_cameraPara[Camera_in_use][g_cameraConstants[Camera_in_use].star].camEuler.yaw, g_cameraPara[Camera_in_use][g_cameraConstants[Camera_in_use].star].camEuler.pitch, g_cameraPara[Camera_in_use][g_cameraConstants[Camera_in_use].star].camEuler.roll);


	// bind to framebuffer and draw scene as we normally would to color texture 
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Need to bind attachment due to change of window size
	// create a color attachment texture
	glBindTexture(GL_TEXTURE_2D, textquad);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_displayWidth, g_displayHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textquad, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_displayWidth, g_displayHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// make sure we clear the framebuffer's content
	glm::vec4 c{ 255.0f, 204.0f, 153.0f, 255.0f };
	c = c / 255.0f;
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// text rendering
	//glEnable(GL_CULL_FACE);
	glm::vec2 scaleV2{ (float)g_displayWidth / (float)g_displayWidthFirst, (float)g_displayHeight / (float)g_displayHeightFirst };
	float fontSize = 2.0f;
	float offsetX = 20.0f;
	unsigned int  lineInterval = (int)max(100 * scaleV2.y, 40);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::vec4 fontColor{ 0.0f, 0.0f, 0.0f, 255.0f };
	fontColor = fontColor / 255.0f;
	for (int i = 0; i < numMsg; i++) {
		textRenderer->Draw(shaderprogram, textbuffer[i], offsetX, (float)(g_displayHeight - (i + 1) * lineInterval), fontSize * scaleV2, fontColor);
	}

	//glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// now draw the attachment texture on the quade
	// disable depth test so screen-space quad isn't discarded due to depth test.
	glDisable(GL_DEPTH_TEST);

	// update quad vertices
	quadVertices = calcQuadVertices(quadX, quadY, dW, dH);

	GLsizei stride{ 4 * sizeof(float) };
	GLuint prog = SolarSys::shader.at("textquad").shaderPtr->ID;
	glUseProgram(prog);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), &quadVertices[0], GL_DYNAMIC_DRAW);
	GLuint texUnitNo = SolarSys::shader.at("textquad").texUnitNo[0];
	glActiveTexture(GL_TEXTURE0 + texUnitNo);
	// use the color attachment texture as the texture of the quad plane
	glBindTexture(GL_TEXTURE_2D, textquad);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);

}

void projection(int w, int h, ProjectionType projectiontype) {

	ProjectionType ptype = projectiontype;
	glm::mat4 projMat{ 1.0f };

	switch (ptype) {
		case Pers:
			projMat = glm::perspective(glm::radians(cameraPtr->Fov), (float)w / (float)h, nearClipPlane, farClipPlane);
			break;
		case PersFOV:
			projMat = glm::perspectiveFov(glm::radians(cameraPtr->Fov), (float)w, (float)h, nearClipPlane, farClipPlane);
			break;
		case InfinitePers:
			projMat = glm::infinitePerspective(glm::radians(cameraPtr->Fov), (float)w/(float)h, nearClipPlane);
		break;
		case Ortho:
			projMat = glm::ortho(0.0f, float(w), 0.0f, float(h));
			break;
		case PixelOrtho://??
			projMat = glm::orthoZO(0.0f, float(w), 0.0f, float(h), nearClipPlane, farClipPlane);
			break;
	}

	//GLuint bindingIndex = SolarSys::shader.at(shadername).unifBlockBindingPoint[1];
	glBindBuffer(GL_UNIFORM_BUFFER, SolarSys::unifBlocks.at("MVPBlock").unifBuffObject);
	glBufferSubData(GL_UNIFORM_BUFFER, SolarSys::mvpSubData.at(SolarSys::subDataType::projection).offset, SolarSys::mvpSubData.at(SolarSys::subDataType::projection).size, glm::value_ptr(projMat));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

};

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.

std::vector<int> keys_TimeControl{
	GLFW_KEY_KP_0,
	GLFW_KEY_KP_1,
	GLFW_KEY_KP_2,
	GLFW_KEY_KP_3,
	GLFW_KEY_KP_4,
};

std::vector<int> keys_Translation{
	GLFW_KEY_X,
	GLFW_KEY_Y,
};

// Callback Keys Setup
std::vector<int> keys_Standard{
	GLFW_KEY_ESCAPE,
	GLFW_KEY_HOME,
	GLFW_KEY_KP_SUBTRACT,
	GLFW_KEY_KP_ADD,
	GLFW_KEY_T,
};

std::vector<int> keys_Viewport{
	GLFW_KEY_F1,
	GLFW_KEY_F2,
	GLFW_KEY_F3,
	GLFW_KEY_F4,
	GLFW_KEY_KP_9,
};

std::vector<int> keys_CameraPosition{
	GLFW_KEY_UP,
	GLFW_KEY_DOWN,
	GLFW_KEY_LEFT,
	GLFW_KEY_RIGHT
};
std::vector<int> keys_Zoom{
	GLFW_KEY_PAGE_UP,
	GLFW_KEY_PAGE_DOWN
};


void keyboard_Timecontrol(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//printf("%c pressed", key);

	switch (key) {
	case GLFW_KEY_KP_0: {
		g_timerControl = Stop;
		break;
	}
	case GLFW_KEY_KP_1: {
		g_timerControl = Slower;
		break;
	}
	case GLFW_KEY_KP_2: {
		g_timerControl = Normal;
		break;
	}
	case GLFW_KEY_KP_3: {
		g_timerControl = Fast;
		break;
	}
	case GLFW_KEY_KP_4: {
		g_timerControl = Faster;
		break;
	}
	case GLFW_KEY_HOME: {
		resetSolarSystem();
		break;
	}
	}
}


void keyboard_Standard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//printf("%c pressed", key);

	switch (key)
	{
	case  GLFW_KEY_ESCAPE: {
		//outFileCamData.close();
		//outFileCamMatData.close();
		//outFileStarData.close();
		//outFileStarMatData.close();
		/*
		delete g_pAxesTintMesh;
		delete g_pCubeMesh;
		delete g_pSphereMesh;
		delete g_pPlaneMesh;
		delete g_pLineMesh;
		g_pAxesTintMesh = NULL;
		g_pCubeMesh = NULL;
		g_pSphereMesh = NULL;
		g_pPlaneMesh = NULL;
		g_pLineMesh = NULL;
		*/
		glfwSetWindowShouldClose(window, true);
		glfwTerminate();
		// Exit program
		exit(EXIT_SUCCESS);
		break;
	}
	case GLFW_KEY_HOME: {
		resetCamera();
		break;
	}

	}

}


void keyboard_CameraPosition(GLFWwindow* window, int key, int scancode, int action, int mods) {

	MyCamera::Camera_Movement direction{};

	switch (key) {
	case GLFW_KEY_UP: {
		direction = MyCamera::Camera_Movement::FORWARD;
		break;
	}
	case GLFW_KEY_DOWN: {
		direction = MyCamera::Camera_Movement::BACKWARD;
		break;
	} 
	case GLFW_KEY_RIGHT: {
		direction = MyCamera::Camera_Movement::RIGHT;
		break;
	}
	case GLFW_KEY_LEFT: {
		direction = MyCamera::Camera_Movement::LEFT;
		break;
	}
	}

	cameraPtr->ProcessKeyboard(direction, deltaTime);

}


CameraID  checkCursorPos(int x, int y) {

	CameraID screen{ EarthCamera };
	int x1{ 0 }, x2{ 0 }, y1{ 0 }, y2{ 0 };

	for (CameraID camera = SolarCamera; camera < Num_CameraID; camera = CameraID(camera + 1)) {

		x1 = vParami[camera][0];
		x2 = vParami[camera][2] + x1;
		y1 = vParami[camera][1];
		y2 = vParami[camera][3] + y1;

		// convert window coordinate too viewport coordinate
		int X = x;
		int Y = g_displayHeight - y;

		if (x1 < X && X < x2 && y1 < Y && Y < y2) {
			screen = camera;
			//std::cout << "Double Click! ::Window Position " << "(x, y) = " << x << ", " << y << " Camera=" << screen  << std::endl;
			break;
		}

	}

	return screen;
}

void callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (glfwGetKey(window, key) != GLFW_PRESS)
		return;

	// just paraphrasing for coding clarification

	std::vector<int>::iterator key_cameraposition = std::find(keys_CameraPosition.begin(), keys_CameraPosition.end(), key);
	if (key_cameraposition != keys_CameraPosition.end()) {
		keyboard_CameraPosition(window, key, scancode, action, mods);
		return;
	}


	std::vector<int>::iterator key_standard = std::find(keys_Standard.begin(), keys_Standard.end(), key);
	if (key_standard != keys_Standard.end()) {
		keyboard_Standard(window, key, scancode, action, mods);
		return;
	}

}


void callback_MouseButton(GLFWwindow* window, int button, int action, int mods) {

	if (button != GLFW_MOUSE_BUTTON_LEFT) {
		// always reset
		clickFirst = true;
		clickSecond = false;
		processOneClick(window, button, action, mods);
		return;
	}

	if (glfwGetMouseButton(window, button) != GLFW_RELEASE) {
		return;
	}

	if (clickSecond) {
		timeAtClickSecond = (float)glfwGetTime();
		diff = timeAtClickSecond - timeAtClickFirst;
		//std::cout << "clickSecond: diff from clickfirst=" << diff <<std::endl;
		if (diff < 0.5f) {
			processDoubleClick(window, button, action, mods);
			return;
		}
		
		// reset
		clickFirst = true;
		clickSecond = false;
		processOneClick(window, button, action, mods);
		return;
	}

	if (clickFirst) {
		timeAtClickFirst = (float)glfwGetTime();
		clickFirst = false;
		clickSecond = true;
		//std::cout << "clickFrist" <<  std::endl;
		return;
	}


	//https://github.com/glfw/glfw/issues/462#:~:text=Double-click%20delay%20is%20saved%20per%20major%20version%20per,system-specific%20things%20to%20read%20the%20proper%20double-click%20delay.

	/* Example!!! double click
	It would be a good idea to have an "howto" on how to get this running on glfw3.This is how I achieved what I wanted in C++11, the DOUBLECLICK stuff is custom :

	void GlfwWindow::mouse_press_callback(int button, int action, int mods) {
		if (action == GLFW_RELEASE) {
			static auto before = std::chrono::system_clock::now();
			auto now = std::chrono::system_clock::now();
			double diff_ms = std::chrono::duration <double, std::milli>(now - before).count();
			before = now;
			if (diff_ms > 10 && diff_ms < 200) {
				action = GLFW_DOUBLECLICK;
				// mDebug() << "doubleclick";
			}
		}
		// ..

		@ataiya Fully support the idea of adding something to the documentation. Just a few thoughts:

	Any reason to use chrono over glfw's own timer support?
	There's no particular need to convert to millis if working in double, since we can compare using fractions of a second.
	I'm not sure looking for successive release events is sufficient for double-click detection. I'd suggest it needs to also look at the presses, but not sure how to make it reliable...
	It needs to check that the button is the same each time. It's not really a double-click if I press two different buttons one after the other.
	Microsoft seems to recommend 500ms as the default click delay, so maybe we should stick to that if providing an example. Unless your 200ms is timing something slightly different?

			*/


}

void processOneClick(GLFWwindow* window, int button, int action, int mods) {

	std::cout << "One Click" << std::endl;
}

void processDoubleClick(GLFWwindow* window, int button, int action, int mods){

	std::cout << "Double Click" << std::endl;

	//CameraID screen;
	//switch (g_screen) {
	//case Num_CameraID: {// all screens

	//	screen = checkCursorPos(cursorPosXf, cursorPosYf);

	//	Camera_in_use = screen;
	//	g_screen = screen;
	//	break;
	//}
	//case SolarCamera: {}
	//case EarthCamera: {}
	//case MoonCamera: {
	//	//std::cout << "Double Click!" << std::endl;
	//	Camera_in_use = SolarCamera;
	//	g_screen = Num_CameraID;
	//	break;
	//}
	//}

}


// glfw: whenever the mouse moves, this callback is called

void callback_CursorPos(GLFWwindow* window, double xpos, double ypos) {

	cursorPosXf = (float)xpos;
	cursorPosYf = (float)ypos;

	//checkCursorPos(cursorPosXf, cursorPosYf);


	if (firstMouse) {
		/*	lastX = xposf;
			lastY = yposf;*/
		firstMouse = false;
	}

	float xoffset = cursorPosXf - lastX;
	float yoffset = lastY - cursorPosYf; // reversed since y-coordinates go from bottom to top

	lastX = cursorPosXf;
	lastY = cursorPosYf;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		cameraPtr->ProcessMouseMovement(xoffset, yoffset);
		return;
	}

	// Move message quad
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		//std::cout << "Mouse Position : X=" << xposf << ", Y=" << yposf << std::endl;
		quadX = 2.0f * cursorPosXf / static_cast<float>(g_displayWidth) - 1.0f;
		quadX = glm::clamp(quadX, -1.0f, 1.0f);
		quadY = -2.0f * cursorPosYf / static_cast<float>(g_displayHeight) + 1.0f;
		quadY = glm::clamp(quadY, -1.0f, 1.0f);
		return;
	}

}


void callback_Scroll(GLFWwindow* window, double xoffset, double yoffset) {

	cameraPtr->ProcessMouseScroll((float)yoffset);

}


void callback_FramebufferSize(GLFWwindow* window, int width, int height) {

	g_framebufferWidth = width;
	g_framebufferHeight = height;

	//std::cout << "FrameBuffer Size : Width=" << width << ", Height=" << height << std::endl;
}

void callback_WindowSize(GLFWwindow* window, int width, int height) {

	g_displayWidth = width;
	g_displayHeight = height;
	//std::cout << "Window Size : Width=" << width << ", Height=" << height << std::endl;

}
