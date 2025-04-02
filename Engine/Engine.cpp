#include "Engine.h"

std::unique_ptr<Engine> GEngine{ std::make_unique<Engine>() };

bool Engine::Init(WindowDesc Desc)
{
	winDesc = std::move(Desc);

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

	//�ֽ� OpenGL ����� ����� �� �ֵ��� ������ ����� Ȱ��ȭ
	glewExperimental = true;
	//GLEW�� �ʱ�ȭ, OpenGL Ȯ�� ��ɵ��� ���
	CHECK(glewInit() == GLEW_OK, "Failed to initialize GLEW", false);

	//Ű �Է��� �߻��ϸ� �ش� Ű�� ���°� ���� �Ⱓ �����ǵ��� �Ͽ�, ���� �� ��ġ�� �Է��� ������
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//Ű �Է� �� ȣ��� �ݹ� �Լ� ���
	glfwSetKeyCallback(window, OpenGLWndProc);

	//�⺻ ���� ����
	glClearColor(winDesc.clearColor.r, winDesc.clearColor.g, winDesc.clearColor.b, winDesc.clearColor.a);

	CHECK(InitProgram(), "Program Init Fail", false);

	return true;
}

int Engine::Run()
{
	static const GLfloat vectexBufferData[]{
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};

	GLuint vertexbuffer;
	//���ؽ� ���� ��ü(VBO)�� ����, ID�Ҵ�
	glGenBuffers(
		1,				//������ ������ ����
		&vertexbuffer);	//������ ���� ID�� ����

	//������ ���۸� Ȱ��ȭ, ���� ���� ���� ȣ�⿡�� �� ���۸� ������� �����ϵ��� ����
	glBindBuffer(GL_ARRAY_BUFFER,		//���� Ÿ��, ���� �����͸� ����
		vertexbuffer);					//���� ��ü�� ID
	//���� �����͸� GPU �޸𸮷� ����
	glBufferData(GL_ARRAY_BUFFER,	//���۵� ���� Ÿ��
		sizeof(vectexBufferData),	//������ �������� ũ��(����Ʈ ����)
		vectexBufferData,			//������ ���� ������
		GL_STATIC_DRAW);			//������ ��� ����(�����Ͱ� ������� �ʰ� GPU�� ����Ǿ� ������ �� �ݺ� ����)

	do {
		//������ ������ ���� ���� �ʱ�ȭ
		glClear(GL_COLOR_BUFFER_BIT);

		//�̸� �����ϵǰ� ��ũ�� ���̴� ���α׷� Ȱ��ȭ
		glUseProgram(programID);

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

		//���ؽ� �迭���� 0�� �ε������� ������ 3���� ���ؽ��� ����Ͽ� �ﰢ���� �׸�
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//�ε��� 0���� ���� �Ӽ� �迭�� ��Ȱ��ȭ, ���� �������� ������ ���� �ʵ��� ����
		glDisableVertexAttribArray(0);

		//�� ���ۿ� ����Ʈ ���۸� ��ü
		glfwSwapBuffers(window);
		//GLFW �̺�Ʈ(Ű �Է�, ���콺 ������ ��)�� ó��
		glfwPollEvents();

	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&		//ESC Ű�� ������ ���� ���
		glfwWindowShouldClose(window) == 0);						//â�� ������ ���� ���

	//GPU �޸𸮿��� ����� ���� ��ü�� ����
	glDeleteBuffers(1,	//������ ������ ����
		&vertexbuffer);	//������ ���� ��ü�� �ּ�
	ShutDown();

	return 0;
}

void Engine::WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{

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

	GLuint vertexShaderID{ EngineUtil::Shader::LoadVertexShader("SimpleVertexShader") };
	GLuint fragmentShaderID{ EngineUtil::Shader::LoadFragmentShader("SimpleFragmentShader") };

	//������ ���α׷� ��ü�� ���̴� ÷��
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//÷�ε� ���̴����� ��ũ�Ͽ� ���� ���� ������ ���̴� ���α׷� ����
	glLinkProgram(programID);

	return EngineUtil::OpenGL::CheckProgram(programID);
}

void Engine::ShutDown()
{
	//������ VAO�� ����
	glDeleteVertexArrays(1,		//������ VAO�� ����
		&vertexArrayID);		//������ VAO�� ID
	//������ ���̴� ���α׷� ��ü�� ����
	glDeleteProgram(programID);

	//GLFW ���̺귯���� ���� �۾��� �����Ͽ�, â, �̺�Ʈ �� ��Ÿ ���� ���ҽ��� ��� ����
	glfwTerminate();
}

void OpenGLWndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	GEngine->WndProc(Window, Key, ScanCode, Action, Mods);
}
