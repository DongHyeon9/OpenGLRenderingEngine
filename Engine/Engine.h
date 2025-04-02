#pragma once
#include "CoreMinimal.h"

extern std::unique_ptr<class Engine> GEngine;

class Engine
{
public:

protected:

private:
	GLFWwindow* window{};
	WindowDesc winDesc{};

	GLuint programID{};
	GLuint vertexArrayID{};

public:
	bool Init(WindowDesc Desc);
	int Run();

	void WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);

protected:

private:
	bool InitProgram();
	void ShutDown();
};

void OpenGLWndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
