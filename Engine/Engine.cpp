#include "Engine.h"

#include "TimerManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "GeometryManager.h"

std::unique_ptr<Engine> GEngine{ std::make_unique<Engine>() };

bool Engine::Init(WindowDesc Desc)
{
	winDesc = std::move(Desc);

	CHECK(InitWindow(), "Window Init Fail", false);
	CHECK(InitOpenGL(), "OpenGL Init Fail", false);
	CHECK(InitProgram(), "Program Init Fail", false);
	CHECK(InitManager(), "Manager Init Fail", false);

	return true;
}

int Engine::Run()
{
	//������ �������Ϳ� MVP��� �̸����� ��Ļ��� �� ID �Ҵ�
	matrixID = glGetUniformLocation(programID, "MVP");

	CHECK(CreateBox(), "Texture Box Fail", -1);
	CHECK(CreateTexture(), "Texture Create Fail", -1);

	while (!glfwWindowShouldClose(window))
	{
		EngineUpdate();
	}

	ShutDown();

	return 0;
}

void Engine::EngineUpdate()
{
	float deltaTime{ TimerManager::GetInstance()->Update() };
	InputManager::GetInstance()->Update();
	CameraManager::GetInstance()->Update(deltaTime);

	Update(deltaTime);
	LateUpdate();

	RenderBegin();
	Render();
	RenderEnd();
}

void Engine::Update(float DeltaTime)
{
	//GLFW �̺�Ʈ(Ű �Է�, ���콺 ������ ��)�� ó��
	glfwPollEvents();

	//�� ��� ����
	glm::mat4 model{ glm::mat4{1.0f} };		//identity ���

	mvp = CameraManager::GetInstance()->GetProjectionMatrix() * CameraManager::GetInstance()->GetViewMatrix() * model;	//column major�� ����

}

void Engine::LateUpdate()
{
}

void Engine::RenderBegin()
{
	//�⺻ ���� ����
	glClearColor(winDesc.clearColor.r, winDesc.clearColor.g, winDesc.clearColor.b, winDesc.clearColor.a);

	//���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);
	//���� �ȼ��� ���� �ȼ����� ���̰� ���� ���(����� ���)�� �����׽�Ʈ ����
	glDepthFunc(GL_LESS);
	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//������ ������ ����, ���� ���� �ʱ�ȭ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Render()
{
	//�̸� �����ϵǰ� ��ũ�� ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(programID);

	glBindVertexArray(vertexArrayID);

	//������ �������Ϳ� �� �Ҵ�
	glUniformMatrix4fv(
		matrixID,		//�Ҵ��� ������ �������� ID
		1,				//������ ����
		GL_FALSE,		//��ġ��� ����� �� ����(column major�� �ۼ��ؼ� �ٲ��� ����)
		&mvp[0][0]		//�������� ���� �ּ�
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, (GLvoid*)(0));

	glBindVertexArray(0);
}

void Engine::RenderEnd()
{
	//�� ���ۿ� ����Ʈ ���۸� ��ü
	glFinish();
	glfwSwapBuffers(window);
}

bool Engine::InitWindow()
{
	//GLFW �ʱ�ȭ
	CHECK(glfwInit(), "glfw Init Fail", false);

	//��Ƽ���ø�(��Ƽ�ٸ����) ���� ���� 4�� ����
	glfwWindowHint(GLFW_SAMPLES, 4);
	//OpenGL ���ؽ�Ʈ�� ������ 3.3���� ����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�ֽ� OpenGL ��� ����� ���� forward compatibility ��带 Ȱ��ȭ
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//OpenGL �ھ� ���������� ����Ͽ� ���ʿ��� ���Ž� ��ɵ��� ��Ȱ��ȭ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//�����츦 ����
	window = glfwCreateWindow(
		winDesc.width,	//����
		winDesc.height, //����
		winDesc.windowTitle.c_str(), //Ÿ��Ʋ
		NULL,
		NULL);	//����Ϳ� ���� ���ؽ�Ʈ ����(�⺻�� ���)
	CHECK(window, "Failed to open GLFW window", false);
	//������ â�� OpenGL ���ؽ�Ʈ�� ���� �����忡 ���ε�, ������ OpenGL ȣ���� �ش� ���ؽ�Ʈ���� ����
	glfwMakeContextCurrent(window);

	//Ű �Է��� �߻��ϸ� �ش� Ű�� ���°� ���� �Ⱓ �����ǵ��� �Ͽ�, ���� �� ��ġ�� �Է��� ������
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//Ű �Է� �� ȣ��� �ݹ� �Լ� ���
	glfwSetKeyCallback(window, WndProc);

	return true;
}

bool Engine::InitOpenGL()
{
	//�ֽ� OpenGL ����� ����� �� �ֵ��� ������ ����� Ȱ��ȭ
	glewExperimental = true;

	//GLEW�� �ʱ�ȭ, OpenGL Ȯ�� ��ɵ��� ���
	CHECK(glewInit() == GLEW_OK, "Failed to initialize GLEW", false);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, nullptr);

	return true;
}

bool Engine::InitProgram()
{
	//OpenGL���� ����� ���ο� ���α׷� ��ü�� ����
	programID = glCreateProgram();

	GLuint vertexShaderID{ EngineUtil::Shader::LoadVertexShader("VS_Default") };
	GLuint fragmentShaderID{ EngineUtil::Shader::LoadFragmentShader("FS_Default") };

	//������ ���α׷� ��ü�� ���̴� ÷��
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//÷�ε� ���̴����� ��ũ�Ͽ� ���� ���� ������ ���̴� ���α׷� ����
	glLinkProgram(programID);

	return EngineUtil::OpenGL::CheckProgram(programID);
}

bool Engine::InitManager()
{
	CHECK(InputManager::GetInstance()->Init(), "Input Manager Init Fail", false);
	CHECK(TimerManager::GetInstance()->Init(), "Timer Manager Init Fail", false);
	CHECK(CameraManager::GetInstance()->Init(), "Camera Manager Init Fail", false);
	CHECK(GeometryManager::GetInstance()->Init(), "Geometry Manager Init Fail", false);

	return true;
}

void Engine::ShutDown()
{
	//GPU �޸𸮿��� ����� ���� ��ü�� ����
	glDeleteBuffers(1,	//������ ������ ����
		&vertexBuffer);	//������ ���� ��ü�� �ּ�
	glDeleteBuffers(1,
		&indexBuffer);

	glDeleteVertexArrays(1, &vertexArrayID);

	glDeleteProgram(programID);

	//GLFW ���̺귯���� ���� �۾��� �����Ͽ�, â, �̺�Ʈ �� ��Ÿ ���� ���ҽ��� ��� ����
	glfwTerminate();
}

bool Engine::CreateBox()
{
	//mesh = GeometryManager::GetInstance()->CreateBox(Vector3(1000.0f));
	//mesh = GeometryManager::GetInstance()->CreateCapsule(1000.0f, 3000.0f, 20, 20);
	mesh = GeometryManager::GetInstance()->CreateCylinder(1000.0f, 1500.0f, 3000.0f, 8);
	//mesh = GeometryManager::GetInstance()->CreateSphere(1000.0f, 20, 20);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// VBO ���� �� ������ ���ε�
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		mesh.vertices.size() * sizeof(Vertex),
		mesh.vertices.data(),
		GL_STATIC_DRAW);

	// 3) ��Ʈ����Ʈ ���� (guard ���)
	auto bindAttrib = [&](const char* name, GLuint size, size_t offset) {
		GLint loc = glGetAttribLocation(programID, name);
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, size, GL_FLOAT,
			GL_FALSE, sizeof(Vertex),
			(void*)offset);
		};
	bindAttrib("position", Vertex::GetPositionSize() / sizeof(float), offsetof(Vertex, position));
	bindAttrib("texcoord", Vertex::GetTexcoordSize() / sizeof(float), offsetof(Vertex, texcoord));
	bindAttrib("normal", Vertex::GetNormalSize() / sizeof(float), offsetof(Vertex, normal));
	bindAttrib("tangent", Vertex::GetTangentSize() / sizeof(float), offsetof(Vertex, tangent));

	// IBO ���� �� ������ ���ε�
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		mesh.indices.size() * sizeof(uint32),
		mesh.indices.data(),
		GL_STATIC_DRAW);

	glBindVertexArray(0);

	return true;
}

bool Engine::CreateTexture()
{
	texture = EngineUtil::Shader::LoadTexture("UVTemplate.bmp");
	textureID = glGetUniformLocation(programID, "textureSampler");
	return true;
}

void WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	InputManager::GetInstance()->UpdateKeyState(Key, Action);
}

void DebugCallback(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam)
{
	static auto LogError = [](const char* Msg) {fprintf(stderr, "[ERROR] %s\n", Msg); };
	static auto LogWarning = [](const char* Msg) { fprintf(stderr, "[WARN ] %s\n", Msg); };
	static auto LogInfo = [](const char* Msg) { fprintf(stdout, "[INFO ] %s\n", Msg); };

	// �߿����� ���� ID ���͸� (�ɼ�)
	if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204)
		return; // non-significant

	// �޽��� ���̺� ����
	const char* type = nullptr;
	switch (Type) {
	case GL_DEBUG_TYPE_ERROR:               type = "ERROR";        break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "DEPRECATED";  break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type = "UNDEFINED";   break;
	case GL_DEBUG_TYPE_PORTABILITY:         type = "PORTABILITY"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type = "PERFORMANCE"; break;
	case GL_DEBUG_TYPE_OTHER:               type = "OTHER";       break;
	default:                               type = "UNKNOWN";     break;
	}

	const char* severity = nullptr;
	switch (Severity) {
	case GL_DEBUG_SEVERITY_HIGH:         severity = "HIGH";         break;
	case GL_DEBUG_SEVERITY_MEDIUM:       severity = "MEDIUM";       break;
	case GL_DEBUG_SEVERITY_LOW:          severity = "LOW";          break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "NOTIFICATION"; break;
	default:                            severity = "UNKNOWN";      break;
	}

	// ���� �б�: Type �Ǵ� Severity ����
	if (Type == GL_DEBUG_TYPE_ERROR || Severity == GL_DEBUG_SEVERITY_HIGH) {
		// ġ���� ����
		LogError(Message);
	}
	else if (Severity == GL_DEBUG_SEVERITY_MEDIUM) {
		// ���
		LogWarning(Message);
	}
	else {
		// ������(LOW / NOTIFICATION)
		LogInfo(Message);
	}
}
