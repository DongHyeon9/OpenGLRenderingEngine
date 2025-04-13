#include "Engine.h"

#include "TimerManager.h"
#include "InputManager.h"
#include "CameraManager.h"

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

	CHECK(CreateBox(), "Box Create Fail", -1);
	CHECK(CreateColor(), "Color Create Fail", -1);
	CHECK(CreateTexture(), "Texture Create Fail", -1);
	CHECK(CreateUV(), "UV Create Fail", -1);

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

	//������ �������Ϳ� �� �Ҵ�
	glUniformMatrix4fv(
		matrixID,		//�Ҵ��� ������ �������� ID
		1,				//������ ����
		GL_FALSE,		//��ġ��� ����� �� ����(column major�� �ۼ��ؼ� �ٲ��� ����)
		&mvp[0][0]		//�������� ���� �ּ�
	);
}

void Engine::LateUpdate()
{
}

void Engine::RenderBegin()
{
	//������ ������ ����, ���� ���� �ʱ�ȭ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Render()
{
	//�̸� �����ϵǰ� ��ũ�� ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(programID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);

	//�ε��� 0���� ���� �Ӽ� �迭�� Ȱ��ȭ
	glEnableVertexAttribArray(0);
	//���� ���� ��ü�� �ٽ� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//���� �������� �Ӽ� ���̾ƿ��� ����
	glVertexAttribPointer(
		0,			//���̴����� ������ ���� �Ӽ��� ��ġ
		3,			//�� ������ ���� 3���� ���(x, y, z)�� ���
		GL_FLOAT,	//�� ����� �ڷ����� float��
		GL_FALSE,	//�����͸� ����ȭ ����(�״�� ���)
		0,			//�� ���� ������ ����(0�̸� �����Ͱ� ���ӵ�)
		(void*)0	//���� ������ ������ ���� ��ġ(0����Ʈ ������)
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//���ؽ� �迭���� 0�� �ε������� ������ 3���� ���ؽ��� ����Ͽ� �ﰢ���� �׸�
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	//�ε��� 0���� ���� �Ӽ� �迭�� ��Ȱ��ȭ, ���� �������� ������ ���� �ʵ��� ����
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Engine::RenderEnd()
{
	//�� ���ۿ� ����Ʈ ���۸� ��ü
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

	//�⺻ ���� ����
	glClearColor(winDesc.clearColor.r, winDesc.clearColor.g, winDesc.clearColor.b, winDesc.clearColor.a);

	//���� �׽�Ʈ Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	//���� �ȼ��� ���� �ȼ����� ���̰� ���� ���(����� ���)�� �����׽�Ʈ ����
	glDepthFunc(GL_LESS);

	return true;
}

bool Engine::InitProgram()
{
	//OpenGL���� ����� ���ο� ���α׷� ��ü�� ����
	programID = glCreateProgram();

	//���ؽ� �Ӽ� �����͸� ����, ������ VAO�� ����
	glGenVertexArrays(1,	//������ VAO(Vertex Array Object)�� ����
		&vertexArrayID);	//������ VAO�� ID
	//���� ������ VAO�� ���� Ȱ��ȭ, ���� ���� ���۳� �Ӽ� ������ �� VAO�� ����
	glBindVertexArray(vertexArrayID);

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

	return true;
}

void Engine::ShutDown()
{
	//GPU �޸𸮿��� ����� ���� ��ü�� ����
	glDeleteBuffers(1,	//������ ������ ����
		&vertexBuffer);	//������ ���� ��ü�� �ּ�
	glDeleteBuffers(1,
		&colorBuffer);

	//������ VAO�� ����
	glDeleteVertexArrays(1,		//������ VAO�� ����
		&vertexArrayID);		//������ VAO�� ID
	//������ ���̴� ���α׷� ��ü�� ����
	glDeleteProgram(programID);

	//GLFW ���̺귯���� ���� �۾��� �����Ͽ�, â, �̺�Ʈ �� ��Ÿ ���� ���ҽ��� ��� ����
	glfwTerminate();
}

bool Engine::CreateBox()
{
	const GLfloat vertexBufferData[]{
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	vertexCount = sizeof(vertexBufferData) / sizeof(GLfloat);
	
	//���ؽ� ���� ��ü(VBO)�� ����, ID�Ҵ�
	glGenBuffers(
		1,				//������ ������ ����
		&vertexBuffer);	//������ ���� ID�� ����

	//������ ���۸� Ȱ��ȭ, ���� ���� ���� ȣ�⿡�� �� ���۸� ������� �����ϵ��� ����
	glBindBuffer(GL_ARRAY_BUFFER,		//���� Ÿ��, ���� �����͸� ����
		vertexBuffer);					//���� ��ü�� ID
	//���� �����͸� GPU �޸𸮷� ����
	glBufferData(GL_ARRAY_BUFFER,							//���۵� ���� Ÿ��
		sizeof(vertexBufferData),			//������ �������� ũ��(����Ʈ ����)
		vertexBufferData,							//������ ���� ������
		GL_STATIC_DRAW);									//������ ��� ����(�����Ͱ� ������� �ʰ� GPU�� ����Ǿ� ������ �� �ݺ� ����)
	
	return true;
}

bool Engine::CreateColor()
{
	const GLfloat colorBufferData[]{
	0.583f,  0.771f,  0.014f,
	0.609f,  0.115f,  0.436f,
	0.327f,  0.483f,  0.844f,
	0.822f,  0.569f,  0.201f,
	0.435f,  0.602f,  0.223f,
	0.310f,  0.747f,  0.185f,
	0.597f,  0.770f,  0.761f,
	0.559f,  0.436f,  0.730f,
	0.359f,  0.583f,  0.152f,
	0.483f,  0.596f,  0.789f,
	0.559f,  0.861f,  0.639f,
	0.195f,  0.548f,  0.859f,
	0.014f,  0.184f,  0.576f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.116f,
	0.676f,  0.977f,  0.133f,
	0.971f,  0.572f,  0.833f,
	0.140f,  0.616f,  0.489f,
	0.997f,  0.513f,  0.064f,
	0.945f,  0.719f,  0.592f,
	0.543f,  0.021f,  0.978f,
	0.279f,  0.317f,  0.505f,
	0.167f,  0.620f,  0.077f,
	0.347f,  0.857f,  0.137f,
	0.055f,  0.953f,  0.042f,
	0.714f,  0.505f,  0.345f,
	0.783f,  0.290f,  0.734f,
	0.722f,  0.645f,  0.174f,
	0.302f,  0.455f,  0.848f,
	0.225f,  0.587f,  0.040f,
	0.517f,  0.713f,  0.338f,
	0.053f,  0.959f,  0.120f,
	0.393f,  0.621f,  0.362f,
	0.673f,  0.211f,  0.457f,
	0.820f,  0.883f,  0.371f,
	0.982f,  0.099f,  0.879f
	};

	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);

	return true;
}

bool Engine::CreateTexture()
{
	texture = EngineUtil::Shader::LoadTexture("UVTemplate.bmp");
	textureID = glGetUniformLocation(programID, "textureSampler");
	return true;
}

bool Engine::CreateUV()
{
	const GLfloat uvBufferData[]{
		0.000059f, 1.0f - 0.000004f,
		0.000103f, 1.0f - 0.336048f,
		0.335973f, 1.0f - 0.335903f,
		1.000023f, 1.0f - 0.000013f,
		0.667979f, 1.0f - 0.335851f,
		0.999958f, 1.0f - 0.336064f,
		0.667979f, 1.0f - 0.335851f,
		0.336024f, 1.0f - 0.671877f,
		0.667969f, 1.0f - 0.671889f,
		1.000023f, 1.0f - 0.000013f,
		0.668104f, 1.0f - 0.000013f,
		0.667979f, 1.0f - 0.335851f,
		0.000059f, 1.0f - 0.000004f,
		0.335973f, 1.0f - 0.335903f,
		0.336098f, 1.0f - 0.000071f,
		0.667979f, 1.0f - 0.335851f,
		0.335973f, 1.0f - 0.335903f,
		0.336024f, 1.0f - 0.671877f,
		1.000004f, 1.0f - 0.671847f,
		0.999958f, 1.0f - 0.336064f,
		0.667979f, 1.0f - 0.335851f,
		0.668104f, 1.0f - 0.000013f,
		0.335973f, 1.0f - 0.335903f,
		0.667979f, 1.0f - 0.335851f,
		0.335973f, 1.0f - 0.335903f,
		0.668104f, 1.0f - 0.000013f,
		0.336098f, 1.0f - 0.000071f,
		0.000103f, 1.0f - 0.336048f,
		0.000004f, 1.0f - 0.671870f,
		0.336024f, 1.0f - 0.671877f,
		0.000103f, 1.0f - 0.336048f,
		0.336024f, 1.0f - 0.671877f,
		0.335973f, 1.0f - 0.335903f,
		0.667969f, 1.0f - 0.671889f,
		1.000004f, 1.0f - 0.671847f,
		0.667979f, 1.0f - 0.335851f
	};

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);

	return true;
}

void WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	InputManager::GetInstance()->UpdateKeyState(Key, Action);
}
