
// This must be included before glfw
#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>
#include <program.h>
#include <SOIL.h>
#include <camera.h>

#include <model.h>
#include <filesystem.h>

using namespace std;

//////////////////////////////////////////Globals /////////////////////////////////
// Window
static GLFWwindow* window = nullptr;
static uint16_t window_width = 800, window_height = 600;

// Program (Shaders and models)
static Program *lampProgram = nullptr, *nanoProgram = nullptr;
static Model *lampModel = nullptr, *nanoModel = nullptr;


// Keyboard and movement
static bool pressedKeys[1024] = {false};
static GLfloat deltaTime, lastFrame;

// Mouse
static bool mousePressed = false;

// Camera
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static GLfloat lastX = 0.0f, lastY = 0.0f;

//Lighing and shading

// Lamp
typedef struct Lamp {

	glm::vec3 position = glm::vec3 (3.0f, +2.75f, 0.0f);
	glm::vec3 color = glm::vec3 ( 1.0f, 1.0f , 1.0f);

	glm::vec3 ambient = color * glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 diffuse = color * glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = color * glm::vec3(1.0f, 1.0f, 1.0f);
}Lamp;

// Object
typedef struct Nano {

	glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0);
	glm::vec3 diffuse = glm::vec3(1.0f, 0.5f, 0.31);
	glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	float shininess = 4.0f;

}Nano;


static Lamp lamp;
static Nano nano;







/////////////////////////////////////////////////////////////////////////////////


//*******************************************************************************//
//					Function Decelerations
//*******************************************************************************//


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_mov_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_press_callback (GLFWwindow*, int button, int action, int mode);


void window_size_callback(GLFWwindow* window, int width, int height);

static void doMovement();
static void	doZoom();

static void updateLights();
static void updateUniforms();

static void drawLamp();
static void drawNano();




//Utilities

enum VecOps {
	INCREASE, DECREASE
};


static inline void
updateVector3f(glm::vec3& vec, enum VecOps op, float byMuch, float limit)
{

	switch (op) {
	case INCREASE:

		vec.x += byMuch; vec.y += byMuch; vec.z += byMuch;
		vec.x = (vec.x >= limit) ? limit : vec.x ;
		vec.y = (vec.y >= limit) ? limit : vec.y ;
		vec.z = (vec.z >= limit) ? limit : vec.z ;
		break;

	case DECREASE:

		vec.x -= byMuch; vec.y -= byMuch; vec.z -= byMuch;
		vec.x = (vec.x <= limit) ? limit : vec.x ;
		vec.y = (vec.y <= limit) ? limit : vec.y ;
		vec.z = (vec.z <= limit) ? limit : vec.z ;

		break;
	}
}

static inline void
prineVector3d(glm::vec3& vec)
{
	cout << vec.x << "-" << vec.y << "-" << vec.z << endl;

}


//****************************************************************************//



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	if (key >= 0 && key <= 1024) {

		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		// This is important - because long press is not considered 'press' event
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}






void mouse_mov_callback(GLFWwindow* window, double xPos, double yPos)
{
	GLfloat xOffset, yOffset;
	static bool firstTime = true;
	if (firstTime) {
		lastX = xPos;
		lastY = yPos;
		firstTime = false;
	}

	xOffset = xPos - lastX;
	yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	if (mousePressed) {
		camera.processMouseMovement(xOffset, yOffset);
	}
}


void mouse_press_callback (GLFWwindow*, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mousePressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mousePressed = false;
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	// TODO (consider using square view port)
}






static void doMovement()
{
	GLfloat doubleTime = (pressedKeys[GLFW_KEY_LEFT_SHIFT]) ? 2.0f : 1.0f;
	GLfloat currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (pressedKeys[GLFW_KEY_W])
		camera.processKeyboardMovement(FORWARD, deltaTime, doubleTime);
	if (pressedKeys[GLFW_KEY_S])
		camera.processKeyboardMovement(BACKWARD, deltaTime, doubleTime);
	if (pressedKeys[GLFW_KEY_A])
		camera.processKeyboardMovement(LEFT, deltaTime, doubleTime);
	if (pressedKeys[GLFW_KEY_D])
		camera.processKeyboardMovement(RIGHT, deltaTime, doubleTime);
	if (pressedKeys[GLFW_KEY_Q])
		camera.processKeyboardMovement(UP, deltaTime, doubleTime);
	if (pressedKeys[GLFW_KEY_Z])
		camera.processKeyboardMovement(DOWN, deltaTime, doubleTime);

}


static void doZoom()
{
	if (pressedKeys[GLFW_KEY_EQUAL])
		camera.processZoom(0.01f);
	if (pressedKeys[GLFW_KEY_MINUS])
		camera.processZoom(-0.01f);

}


static void
updateLights()
{


	if (pressedKeys[GLFW_KEY_8]) {
		updateVector3f(nano.ambient, INCREASE, 0.002, 1.0);
	}
	if (pressedKeys[GLFW_KEY_7]) {
		updateVector3f(nano.ambient, DECREASE, 0.002, 0.0);
	}

	if (pressedKeys[GLFW_KEY_6]) {
		if (nano.shininess < 1024) {
			nano.shininess *= 2.0f;
		}
	}
	if (pressedKeys[GLFW_KEY_5]) {
		if (nano.shininess > 1) {
			nano.shininess /= 2.0f;
		}
	}
}


static void updateUniforms()
{


	// For Lamp module
	lampProgram->use();
	lampProgram->setVec3("lamp.position", lamp.position.x, lamp.position.y, lamp.position.z);
	lampProgram->setVec3("lamp.color", lamp.color.x, lamp.color.y, lamp.color.z);

	// For Nano Module
	nanoProgram->use();
	nanoProgram->setVec3("viewPos", camera.position);
	nanoProgram->setVec3("light.position", lamp.position);
	nanoProgram->setVec3("light.ambient", lamp.ambient);
	nanoProgram->setVec3("light.diffuse", lamp.diffuse);
	nanoProgram->setVec3("light.specular", lamp.specular);
	nanoProgram->setVec3("material.color", 1.0f, 1.0f, 1.0f);
	nanoProgram->setVec3("material.ambient", nano.ambient);
	nanoProgram->setVec3("material.diffuse", nano.diffuse);
	nanoProgram->setVec3("material.specular", nano.specular);
	nanoProgram->setFloat("material.shininess", nano.shininess);

}




static void
drawLamp()
{

//	lamp.color.x = max(sin(glfwGetTime() * 0.5f), 0.0f);
//	lamp.color.y = max(cos(glfwGetTime() * 0.3f), 0.0f);
//	lamp.color.z = max(cos(glfwGetTime() * 0.3f), 0.0f);


	lamp.ambient = lamp.color * glm::vec3(0.5f, 0.5f, 0.5f);
	lamp.diffuse = lamp.color * glm::vec3(0.5f, 0.5f, 0.5f);
	lamp.specular = lamp.color * glm::vec3(1.0f, 1.0f, 1.0f);

	lamp.position.x = sin(glfwGetTime() * 0.5f);
	lamp.position.z = cos(glfwGetTime() * 0.5f);




	lampProgram->use();   // <-- Don't forget this one!
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.zoom, (float)window_width/(float)window_height, 0.1f, 100.0f);
	glm::mat4 view = camera.getViewMatrix();

	lampProgram->setMat4("projection", projection);
	lampProgram->setMat4("view", view);

	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, lamp.position); // Translate it down a bit so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down

	lampProgram->setMat4("model", model);
	lampModel->draw(*lampProgram);
}

static void drawNano()
{

	nanoProgram->use();   // <-- Don't forget this one!
	// Transformation matrices
	glm::mat4 projection = glm::perspective(camera.zoom, (float)window_width/(float)window_height, 0.1f, 100.0f);
	glm::mat4 view = camera.getViewMatrix();

	nanoProgram->setMat4("projection", projection);
	nanoProgram->setMat4("view", view);


	// Draw the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f)); // Translate it up a bit so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// It's a bit too big for our scene, so scale it down

	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

	nanoProgram->setMat4("model", model);
	nanoProgram->setMat3("normalMatrix", normalMatrix);
	nanoModel->draw(*nanoProgram);
}




int main(int argc, char* argv[])
{
	// init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Allow resize of window (we add the callback now)
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(window_width, window_height, "Project 1", nullptr, nullptr);
	if (!window) {

		cerr << "Unable to create GLFW window for project 1." << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);


	// init glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glViewport(0, 0, window_width, window_height);

	// Enable Z-Buffer tests
	glEnable(GL_DEPTH_TEST);


	//set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_mov_callback);
	glfwSetMouseButtonCallback(window, mouse_press_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);


	// load Shaders
	lampProgram = new Program("shaders/lampShader.vs", "shaders/lampShader.frag");
	nanoProgram = new Program("shaders/nanoShader.vs", "shaders/nanoShader.frag");

	// load Models
	lampModel = new Model(FileSystem::getPath("Project_1/resources/objects/cube/cube.obj").c_str());
	nanoModel = new Model(FileSystem::getPath("Project_1/resources/objects/nanosuit/nanosuit.obj").c_str());


	// Game loop
	while(!glfwWindowShouldClose(window))
	{
		// Clear the colorbuffer
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Check and call events
		glfwPollEvents();

		doMovement();
		doZoom();

		updateLights();
		updateUniforms();

		drawLamp();
		drawNano();

		// Swap the buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();


	delete lampProgram; delete nanoProgram;
	delete lampModel; delete nanoModel;


	return 0;
}








