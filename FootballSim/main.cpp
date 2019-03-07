#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <assimp/Importer.hpp>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Ball.h"
#include "Texture.h"
#include "Light.h"

#include "Model.h"

float rotate = 0.0f;

// Window dimensions
const GLint WIDTH = 1920, HEIGHT = 1080;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Shader> shaderList;
Camera camera;

Model nanosuit;
Model stadiumMod;
Model ballMod;

Light mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

glm::vec3 spinDirection = glm::vec3(0.0f, 1.0f, 0.0f);
float angularVelocity123 = 80.0f;

// Vertex Shader code
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	//W,Vb,xAngle,yAngle,spinDir
	Ball myBall(angularVelocity123, 36.0f, 0.0f, 45.0f, spinDirection);
	

	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	nanosuit = Model();
	nanosuit.LoadModel("Models/nanosuit.obj");

	stadiumMod = Model();
	stadiumMod.LoadModel("Models/Stadium.obj");

	ballMod = Model();
	ballMod.LoadModel("Models/soccerball.obj");

	mainLight = Light(1.0f, 1.0f, 1.0f, 0.4f, 0.5f, -1.0f, 0.8f, 0.6f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
	GLuint uniformAmbientIntensity = 0, uniformAmbientColour = 0, uniformDirection = 0, uniformDiffuseIntensity = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	
	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		// Get + Handle user input events

		lastTime = now;

		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());


		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformAmbientColour = shaderList[0].GetAmbientColourLocation();
		uniformAmbientIntensity = shaderList[0].GetAmbientIntensityLocation();
		uniformDirection = shaderList[0].GetDirectionLocation();
		uniformDiffuseIntensity = shaderList[0].GetDiffuseIntensityLocation();

		mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColour, uniformDiffuseIntensity, uniformDirection);

		glm::mat4 model = glm::mat4(1.0f);
		bool* keys = mainWindow.getsKeys();
		if (keys[GLFW_KEY_F])
		{
			myBall.kick();
		}
		if (myBall.getHasBeenKicked())
		{
			model = glm::translate(model, myBall.euler(deltaTime));
			rotate -= myBall.getAngularVelocity() / 1000.0f;
			spinDirection = myBall.getSpinDirection();
			model = glm::rotate(model, rotate, spinDirection);
		}

		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		ballMod.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		nanosuit.RenderModel();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-80.0f, 0.0f, 130.0f));
		model = glm::scale(model, glm::vec3(0.0017f, 0.0015f, 0.0017f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		stadiumMod.RenderModel();
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}