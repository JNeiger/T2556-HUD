// OpenGL_BasicGUI.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <Windows.h>
#include <math.h>

//Include GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//OpenGL Headers
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
//#include <GL\glut.h>
#include "shader.hpp"
#include "controls.hpp"
#include "GL/gl.h"

//GLFont2 Headers
#include "glfont2/glfont2.h"

//FPS Headers
#include <ctime>


const std::string SL = "..\\OpenGL_BasicGUI\\Shaders\\";

struct matchData {
	int time;
	float fps;
};

GLFWwindow* window;
int width, height;
float camera_x, camera_y, camera_z;
float hud_length;
glfont::GLFont font;

matchData match;

static void error_callback(int error, const char* description);
void modelMatrix(GLuint MatrixID);

void OpenGLReady3D();
void OpenGLReady2D();
void OpenGLStart();
void OpenGLFont2();
void OpenGLSetView();
void OpenGLRender2D();
void OpenGLRender3D();

void DrawString(std::string s, int width, int height, float r, float g, float b, float scale);
void StartGLFont2();
void StopGLFont2();

void InitMatchData();
void GetMatchData();

int _tmain(int argc, _TCHAR* argv[])
{
	OpenGLStart();
	OpenGLFont2();

	InitMatchData();

	for (int i = 0; i < 500; i++) 
	{
		clock_t start = clock();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GetMatchData();

		OpenGLRender2D();
		OpenGLRender3D();
		
		glfwSwapBuffers(window);

		camera_x = cosf( (float) i / 500.0 * 2 * 3.14 ) * 200;
		camera_y = sinf( (float) i / 500.0 * 2 * 3.14 ) * 200;
		camera_z = sinf( (float) i / 500.0 * 2 * 3.14 ) * 50 + 250;
		hud_length =  (-0.5 * cosf( (float) i / 500.0 * 2 * 3.14 ) + .5 ) * width;
		Sleep(10);


		clock_t end = clock();
		match.fps = 1.0 / (double(end - start) / CLOCKS_PER_SEC);
	}
	return 0;
}

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}


void modelMatrix(GLuint MatrixID)
{
	// Model matrix : an identity matrix (model will be at the origin)
	computeMatricesFromInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}

void OpenGLReady3D()
{
	// Cull back faces and layer correctly
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	// Set view width, height and type
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);

	// Set up 3D view with given width, hight, near and far restrictrions
	glLoadIdentity();
	gluPerspective(45, (float)width / height, 0.1, 5000.0);
	gluLookAt(camera_x, camera_y, camera_z, 0, 0, 0, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLReady2D()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up orthographic view for HUD view
	gluOrtho2D(0.0f, width, 0.0f, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.375, 0.375, 0.0);
}

void OpenGLStart()
{
	width = 960;
	height = 720;
	camera_z = 200;

	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Create a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Test Window", NULL, NULL);

	//If the window couldn't be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		//exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	//Sets the key callback
	// glfwSetKeyCallback(window, key_callback);

	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return;
	}
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	OpenGLSetView();
}

void OpenGLFont2()
{
	GLuint tex;
	glGenTextures(1, &tex);
	if (!font.Create("glfont2\\arial.glf", tex))
		printf("Error: Couldn't create font texture\n");
}

void OpenGLSetView()
{
	GLuint grid = LoadShaders((SL + "SimpleVertexShader.vertexshader").c_str(), (SL + "SimpleFragmentShader.fragmentshader").c_str());
	GLuint MatrixID = glGetUniformLocation(grid, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(grid, "V");
	GLuint ModelMatrixID = glGetUniformLocation(grid, "M");

	modelMatrix(MatrixID);
}

void OpenGLRender3D()
{
	OpenGLReady3D();

	// Draw 3D Scene here
	glColor3f(0.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex3f(100, -50, 0);
		glVertex3f(100, 50, 0);
		glVertex3f(-100, 50, 0);
		glVertex3f(-100, -50, 0);
	glEnd();
}

void OpenGLRender2D()
{
	OpenGLReady2D();

	// Draw 2D Scene here
	
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(0, 100);
		glVertex2f(hud_length, 100);
		glVertex2f(hud_length, 0);
	glEnd();

	glColor3f(.2, .2, .2);
	for (int i = 0; i < width; i += width - 100)
	{
		glBegin(GL_QUADS);
			glVertex2f(i, 0);
			glVertex2f(i + 100, 0);
			glVertex2f(i + 100, height);
			glVertex2f(i, height);
		glEnd();
	}
	
	glBegin(GL_QUADS);
	glVertex2d(width * 1/3, height);
	glVertex2d(width * 1/3, height * 8/9);
	glVertex2d(width * 2/3, height * 8/9);
	glVertex2d(width * 2/3, height);
	glEnd();

	StartGLFont2();
		//DrawString("test", 0, 500, 0.0, 1.0, 0.0, 1);
		DrawString("test", 0, 500, 0.0, 1.0, 0.0, 1.0);

		std::string matchTime = std::to_string(match.time);
		DrawString(matchTime, width * 1 / 2, height * 100 / 105, 1.0, 1.0, 1.0, 1);

		DrawString(std::to_string(match.fps) + " FPS", 0, height, 1.0, 1.0, 1.0, 0.5);
	StopGLFont2();
}

void DrawString(std::string s, int width, int height, float r, float g, float b, float scale)
{
	glScalef(scale, scale, 1);
	glColor3f(r, g, b);
	font.DrawString(s, width * (1 / scale), height * (1 / scale));
	glScalef((1 / scale), (1 / scale), 1);
}

int GetStringLengthInP(std::string s)
{
	int sum = 0;

	for (char& c : s)
	{
		//int i = c - font.IntStart;
	}

	return 0;
}

void StartGLFont2()
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void StopGLFont2()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void InitMatchData()
{
	match.time = 500;
}

void GetMatchData()
{
	match.time--;
}