////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "CSE 169 Starter";

// Objects to render
Cloth* Window::cloth;
Ground* Window::ground;

// Ground constants
int Window::size = 100;
float Window::elevation = -5.0f;

// Cloth constants
int Window::length = 4;
float Window::springConstant = 500;
float Window::damperConstant = 60;
float Window::dragCoeff = 2;
float Window::restitution = 0.05;
float Window::frictionCoeff = 2;

// cloth movement;
float step = 0.5f;

// gravity
glm::vec3 Window::gravity = glm::vec3(0, -9.8, 0);

// wind
glm::vec3 Window::windSpeed = glm::vec3(0, 0, 0);
float Window::density = 1.225;
float mag = 2;	// magnitude of wind
glm::vec3 windInc = glm::vec3(0, 0, mag);	// direction of wind
glm::vec3 windIncZ = glm::vec3(0, 0, mag);
glm::vec3 windIncY = glm::vec3(0, mag, 0);
glm::vec3 windIncX = glm::vec3(mag, 0, 0);

// animation speed
float Window::deltaTime = 0.0002;
int Window::oversamples = 100;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;


////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	cloth = new Cloth(length, springConstant, damperConstant, dragCoeff, restitution, frictionCoeff);
	ground = new Ground(size, elevation);


	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cloth;
	delete ground;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

////////////////////////////////////////////////////////////////////////////////

// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////

// update and draw functions
void Window::idleCallback()
{
	// Perform any updates as necessary. 
	Cam->Update();

	// oversample cloth animation
	for (int i = 0; i < oversamples; i++) {
		cloth->update(deltaTime, gravity, windSpeed, density);
		cloth->checkGroundCollision(ground);
	}
	cloth->updateBuffer();

}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the object.
	cloth->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	ground->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}

////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;
			
		case GLFW_KEY_W:
			cloth->translate(glm::vec3(0, 0, -step));
			break;
			
		case GLFW_KEY_S:
			cloth->translate(glm::vec3(0, 0, step));
			break;
			
		case GLFW_KEY_A:
			cloth->translate(glm::vec3(-step, 0, 0));
			break;
			
		case GLFW_KEY_D:
			cloth->translate(glm::vec3(step, 0, 0));
			break;
			
		case GLFW_KEY_E:
			cloth->translate(glm::vec3(0, step, 0));
			break;
			
		case GLFW_KEY_Q:
			cloth->translate(glm::vec3(0, -step, 0));
			break;

		case GLFW_KEY_1:
			windInc = windIncZ;
			cout << "Wind adjustment direction: Z" << endl;
			break;

		case GLFW_KEY_2:
			windInc = windIncY;
			cout << "Wind adjustment direction: Y" << endl;
			break;

		case GLFW_KEY_3:
			windInc = windIncX;
			cout << "Wind adjustment direction: X" << endl;
			break;

		case GLFW_KEY_L:
			windSpeed += windInc;
			cout << "Wind Speed: ( " << windSpeed.x << ", " << windSpeed.y << ", " << windSpeed.z << ")" << endl;
			break;

		case GLFW_KEY_K:
			windSpeed -= windInc;
			cout << "Wind Speed: ( " << windSpeed.x << ", " << windSpeed.y << ", " << windSpeed.z << ")" << endl;
			break;

		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////