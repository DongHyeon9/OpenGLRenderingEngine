#include "Engine.h"

std::unique_ptr<Engine> GEngine{ std::make_unique<Engine>() };

bool Engine::Init(WindowDesc Desc)
{
	winDesc = std::move(Desc);

	CHECK(InitWindow(), "Window Init Fail", false);
	CHECK(InitOpenGL(), "OpenGL Init Fail", false);
	CHECK(InitProgram(), "Program Init Fail", false);

	return true;
}

int Engine::Run()
{
	//������ �������Ϳ� MVP��� �̸����� ��Ļ��� �� ID �Ҵ�
	GLuint matrixID = glGetUniformLocation(programID, "MVP");

	CHECK(CreateBox(), "Box Create Fail", -1);
	CHECK(CreateColor(), "Color Create Fail", -1);

	//������� ����
	glm::mat4 projection{ glm::perspective(
		glm::radians(90.0f),									//fov
		static_cast<float>(winDesc.width) / winDesc.height,		//aspect
		0.1f,													//near Z
		100.0f)													//far Z
	};

	//�� ��� ����
	glm::mat4 view{ glm::lookAt(
		glm::vec3{4,3,3},		//ī�޶� ��ġ
		glm::vec3{0,0,0},		//ī�޶� ���� ��
		glm::vec3{0,0,1})		//ī�޶��� up����
	};

	//�� ��� ����
	glm::mat4 model{ glm::mat4{1.0f} };		//identity ���

	glm::mat4 mvp = projection * view * model;	//column major�� ����

	do {
		//������ ������ ����, ���� ���� �ʱ�ȭ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//�̸� �����ϵǰ� ��ũ�� ���̴� ���α׷� Ȱ��ȭ
		glUseProgram(programID);

		//������ �������Ϳ� �� �Ҵ�
		glUniformMatrix4fv(
			matrixID,		//�Ҵ��� ������ �������� ID
			1,				//������ ����
			GL_FALSE,		//��ġ��� ����� �� ����(column major�� �ۼ��ؼ� �ٲ��� ����)
			&mvp[0][0]		//�������� ���� �ּ�
		);

		//�ε��� 0���� ���� �Ӽ� �迭�� Ȱ��ȭ
		glEnableVertexAttribArray(0);
		//���� ���� ��ü�� �ٽ� ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
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

		//�� ���ۿ� ����Ʈ ���۸� ��ü
		glfwSwapBuffers(window);
		//GLFW �̺�Ʈ(Ű �Է�, ���콺 ������ ��)�� ó��
		glfwPollEvents();

	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&		//ESC Ű�� ������ ���� ���
		glfwWindowShouldClose(window) == 0);						//â�� ������ ���� ���

	ShutDown();

	return 0;
}

void Engine::WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{

}

bool Engine::InitWindow()
{
	//GLFW �ʱ�ȭ
	CHECK(glfwInit(), "glfw Init Fail", false);

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
	glfwSetKeyCallback(window, OpenGLWndProc);

	return true;
}

bool Engine::InitOpenGL()
{
	//��Ƽ���ø�(��Ƽ�ٸ����) ���� ���� 4�� ����
	glfwWindowHint(GLFW_SAMPLES, 4);
	//OpenGL ���ؽ�Ʈ�� ������ 3.3���� ����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�ֽ� OpenGL ��� ����� ���� forward compatibility ��带 Ȱ��ȭ
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//OpenGL �ھ� ���������� ����Ͽ� ���ʿ��� ���Ž� ��ɵ��� ��Ȱ��ȭ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

void Engine::ShutDown()
{
	//GPU �޸𸮿��� ����� ���� ��ü�� ����
	glDeleteBuffers(1,	//������ ������ ����
		&vertexbuffer);	//������ ���� ��ü�� �ּ�
	glDeleteBuffers(1,
		&colorbuffer);

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
		&vertexbuffer);	//������ ���� ID�� ����

	//������ ���۸� Ȱ��ȭ, ���� ���� ���� ȣ�⿡�� �� ���۸� ������� �����ϵ��� ����
	glBindBuffer(GL_ARRAY_BUFFER,		//���� Ÿ��, ���� �����͸� ����
		vertexbuffer);					//���� ��ü�� ID
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

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData, GL_STATIC_DRAW);

	return true;
}

void OpenGLWndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	GEngine->WndProc(Window, Key, ScanCode, Action, Mods);
}
