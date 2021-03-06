#include <iostream>
#include <exception>
#include <string>
#include "Game/Game.h"
#include "ResourceManager.h"
#include "DebugColors.h"
#include "config.h"
#include <GLFW/glfw3.h>

/* Needed to compile with static SFML-audio library. Otherwise there will be linking problems with flac.lib etc. */
#if WIN32
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}
#endif


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << DC_ERROR " Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);

	// OpenGL configuration
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	try 
	{
		// Initialize game
		Breakout.Init();	

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = (float)glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			glfwPollEvents();

			Breakout.ProccessInput(deltaTime);
			Breakout.Update(deltaTime);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			Breakout.Render();

			glfwSwapBuffers(window);
		}

		Breakout.Destroy();
	}
	catch (std::exception *e)
	{
		std::cerr << "Error: " << e->what() << std::endl;
	}

	ResourceManager::Clear();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCREEN_HEIGHT = height;
	SCREEN_WIDTH = width;
	glViewport(0, 0, width, height);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			Breakout.Keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			Breakout.Keys[key] = false;
			Breakout.KeysProcessed[key] = false;
		}
	}
}