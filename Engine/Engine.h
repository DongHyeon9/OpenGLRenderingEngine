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
	GLuint vertexBuffer{};
	GLuint indexBuffer{};
	MeshData mesh{};

	//GLuint vertexBuffer{};
	//GLuint colorBuffer{};
	//GLuint uvBuffer{};
	//GLsizei vertexCount{};

	GLuint texture{};
	GLuint textureID{};

	glm::mat4 mvp{};
	GLuint matrixID{};

public:
	bool Init(WindowDesc Desc);
	int Run();
	void EngineUpdate();

	int GetWindowWidth()const { return winDesc.width; }
	int GetWindowHeight()const { return winDesc.width; }
	glm::vec2 GetWindowSize()const { return glm::vec2{ static_cast<float>(winDesc.width) ,static_cast<float>(winDesc.height) }; }

protected:

private:
	bool InitWindow();
	bool InitOpenGL();
	bool InitProgram();
	bool InitManager();
	void ShutDown();

	void Update(float DeltaTime);
	void LateUpdate();
	void RenderBegin();
	void Render();
	void RenderEnd();

	bool CreateBox();
	bool CreateTexture();
};

void WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods);
void APIENTRY DebugCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam);
