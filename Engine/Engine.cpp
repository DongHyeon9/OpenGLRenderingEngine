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
	//유니폼 레지스터에 MVP라는 이름으로 행렬생성 및 ID 할당
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
	//GLFW 이벤트(키 입력, 마우스 움직임 등)를 처리
	glfwPollEvents();
	
	//모델 행렬 생성
	glm::mat4 model{ glm::mat4{1.0f} };		//identity 행렬

	mvp = CameraManager::GetInstance()->GetProjectionMatrix() * CameraManager::GetInstance()->GetViewMatrix() * model;	//column major에 유의

	//유니폼 레지스터에 값 할당
	glUniformMatrix4fv(
		matrixID,		//할당할 유니폼 레지스터 ID
		1,				//데이터 갯수
		GL_FALSE,		//전치행렬 사용할 지 여부(column major로 작성해서 바꾸지 않음)
		&mvp[0][0]		//데이터의 시작 주소
	);
}

void Engine::LateUpdate()
{
}

void Engine::RenderBegin()
{
	//프레임 버퍼의 색상, 깊이 정보 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Render()
{
	//미리 컴파일되고 링크된 쉐이더 프로그램 활성화
	glUseProgram(programID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);

	//인덱스 0번의 정점 속성 배열을 활성화
	glEnableVertexAttribArray(0);
	//정점 버퍼 객체를 다시 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//정점 데이터의 속성 레이아웃을 지정
	glVertexAttribPointer(
		0,			//쉐이더에서 참조할 정점 속성의 위치
		3,			//각 정점에 대해 3개의 요소(x, y, z)가 사용
		GL_FLOAT,	//각 요소의 자료형이 float임
		GL_FALSE,	//데이터를 정규화 여부(그대로 사용)
		0,			//각 정점 사이의 간격(0이면 데이터가 연속됨)
		(void*)0	//버퍼 내에서 데이터 시작 위치(0바이트 오프셋)
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

	//버텍스 배열에서 0번 인덱스부터 시작해 3개의 버텍스를 사용하여 삼각형을 그림
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	//인덱스 0번의 정점 속성 배열을 비활성화, 이후 렌더링에 영향을 주지 않도록 설정
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Engine::RenderEnd()
{
	//백 버퍼와 프론트 버퍼를 교체
	glfwSwapBuffers(window);
}

bool Engine::InitWindow()
{
	//GLFW 초기화
	CHECK(glfwInit(), "glfw Init Fail", false);

	//멀티샘플링(안티앨리어싱) 샘플 수를 4로 지정
	glfwWindowHint(GLFW_SAMPLES, 4);
	//OpenGL 컨텍스트의 버전을 3.3으로 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//최신 OpenGL 기능 사용을 위해 forward compatibility 모드를 활성화
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//OpenGL 코어 프로파일을 사용하여 불필요한 레거시 기능들을 비활성화
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//윈도우를 생성
	window = glfwCreateWindow(
		winDesc.width,	//가로
		winDesc.height, //세로
		winDesc.windowTitle.c_str(), //타이틀
		NULL,
		NULL);	//모니터와 공유 컨텍스트 설정(기본값 사용)
	CHECK(window, "Failed to open GLFW window", false);
	//생성된 창의 OpenGL 컨텍스트를 현재 쓰레드에 바인딩, 이후의 OpenGL 호출이 해당 컨텍스트에서 수행
	glfwMakeContextCurrent(window);

	//키 입력이 발생하면 해당 키의 상태가 일정 기간 유지되도록 하여, 폴링 시 놓치는 입력이 없게함
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//키 입력 시 호출될 콜백 함수 등록
	glfwSetKeyCallback(window, WndProc);

	return true;
}

bool Engine::InitOpenGL()
{
	//최신 OpenGL 기능을 사용할 수 있도록 실험적 기능을 활성화
	glewExperimental = true;

	//GLEW를 초기화, OpenGL 확장 기능들을 사용
	CHECK(glewInit() == GLEW_OK, "Failed to initialize GLEW", false);

	//기본 배경색 설정
	glClearColor(winDesc.clearColor.r, winDesc.clearColor.g, winDesc.clearColor.b, winDesc.clearColor.a);

	//깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);

	//현재 픽셀이 이전 픽셀보다 깊이가 낮을 경우(가까울 경우)로 깊이테스트 진행
	glDepthFunc(GL_LESS);

	return true;
}

bool Engine::InitProgram()
{
	//OpenGL에서 사용할 새로운 프로그램 객체를 생성
	programID = glCreateProgram();

	//버텍스 속성 데이터를 저장, 관리할 VAO를 생성
	glGenVertexArrays(1,	//생성할 VAO(Vertex Array Object)의 개수
		&vertexArrayID);	//생성된 VAO의 ID
	//새로 생성한 VAO를 현재 활성화, 이후 정점 버퍼나 속성 설정은 이 VAO에 적용
	glBindVertexArray(vertexArrayID);

	GLuint vertexShaderID{ EngineUtil::Shader::LoadVertexShader("VS_Default") };
	GLuint fragmentShaderID{ EngineUtil::Shader::LoadFragmentShader("FS_Default") };

	//생성된 프로그램 객체에 쉐이더 첨부
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//첨부된 쉐이더들을 링크하여 최종 실행 가능한 쉐이더 프로그램 생성
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
	//GPU 메모리에서 사용한 버퍼 객체를 해제
	glDeleteBuffers(1,	//삭제할 버퍼의 개수
		&vertexBuffer);	//삭제할 버퍼 객체의 주소
	glDeleteBuffers(1,
		&colorBuffer);

	//생성된 VAO를 삭제
	glDeleteVertexArrays(1,		//삭제할 VAO의 개수
		&vertexArrayID);		//삭제할 VAO의 ID
	//생성된 쉐이더 프로그램 객체를 삭제
	glDeleteProgram(programID);

	//GLFW 라이브러리의 종료 작업을 수행하여, 창, 이벤트 및 기타 관련 리소스를 모두 정리
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
	
	//버텍스 버퍼 객체(VBO)를 생성, ID할당
	glGenBuffers(
		1,				//생성할 버퍼의 개수
		&vertexBuffer);	//생성된 버퍼 ID를 저장

	//생성된 버퍼를 활성화, 이후 버퍼 관련 호출에서 이 버퍼를 대상으로 동작하도록 만듬
	glBindBuffer(GL_ARRAY_BUFFER,		//버퍼 타입, 정점 데이터를 저장
		vertexBuffer);					//버퍼 객체의 ID
	//정점 데이터를 GPU 메모리로 복사
	glBufferData(GL_ARRAY_BUFFER,							//전송될 버퍼 타입
		sizeof(vertexBufferData),			//전송할 데이터의 크기(바이트 단위)
		vertexBufferData,							//전송할 실제 데이터
		GL_STATIC_DRAW);									//데이터 사용 패턴(데이터가 변경되지 않고 GPU에 저장되어 렌더링 시 반복 사용됨)
	
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
