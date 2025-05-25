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
	//유니폼 레지스터에 MVP라는 이름으로 행렬생성 및 ID 할당
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
	//GLFW 이벤트(키 입력, 마우스 움직임 등)를 처리
	glfwPollEvents();

	//모델 행렬 생성
	glm::mat4 model{ glm::mat4{1.0f} };		//identity 행렬

	mvp = CameraManager::GetInstance()->GetProjectionMatrix() * CameraManager::GetInstance()->GetViewMatrix() * model;	//column major에 유의

}

void Engine::LateUpdate()
{
}

void Engine::RenderBegin()
{
	//기본 배경색 설정
	glClearColor(winDesc.clearColor.r, winDesc.clearColor.g, winDesc.clearColor.b, winDesc.clearColor.a);

	//깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST);
	//현재 픽셀이 이전 픽셀보다 깊이가 낮을 경우(가까울 경우)로 깊이테스트 진행
	glDepthFunc(GL_LESS);
	glDepthRange(0.0F, 1.0F);
	glClearDepthf(1.0F);

	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//프레임 버퍼의 색상, 깊이 정보 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::Render()
{
	//미리 컴파일되고 링크된 쉐이더 프로그램 활성화
	glUseProgram(programID);

	glBindVertexArray(vertexArrayID);

	//유니폼 레지스터에 값 할당
	glUniformMatrix4fv(
		matrixID,		//할당할 유니폼 레지스터 ID
		1,				//데이터 갯수
		GL_FALSE,		//전치행렬 사용할 지 여부(column major로 작성해서 바꾸지 않음)
		&mvp[0][0]		//데이터의 시작 주소
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, (GLvoid*)(0));

	glBindVertexArray(0);
}

void Engine::RenderEnd()
{
	//백 버퍼와 프론트 버퍼를 교체
	glFinish();
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

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, nullptr);

	return true;
}

bool Engine::InitProgram()
{
	//OpenGL에서 사용할 새로운 프로그램 객체를 생성
	programID = glCreateProgram();

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
	CHECK(GeometryManager::GetInstance()->Init(), "Geometry Manager Init Fail", false);

	return true;
}

void Engine::ShutDown()
{
	//GPU 메모리에서 사용한 버퍼 객체를 해제
	glDeleteBuffers(1,	//삭제할 버퍼의 개수
		&vertexBuffer);	//삭제할 버퍼 객체의 주소
	glDeleteBuffers(1,
		&indexBuffer);

	glDeleteVertexArrays(1, &vertexArrayID);

	glDeleteProgram(programID);

	//GLFW 라이브러리의 종료 작업을 수행하여, 창, 이벤트 및 기타 관련 리소스를 모두 정리
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

	// VBO 생성 및 데이터 업로드
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		mesh.vertices.size() * sizeof(Vertex),
		mesh.vertices.data(),
		GL_STATIC_DRAW);

	// 3) 어트리뷰트 설정 (guard 사용)
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

	// IBO 생성 및 데이터 업로드
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

	// 중요하지 않은 ID 필터링 (옵션)
	if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204)
		return; // non-significant

	// 메시지 레이블 생성
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

	// 실제 분기: Type 또는 Severity 기준
	if (Type == GL_DEBUG_TYPE_ERROR || Severity == GL_DEBUG_SEVERITY_HIGH) {
		// 치명적 에러
		LogError(Message);
	}
	else if (Severity == GL_DEBUG_SEVERITY_MEDIUM) {
		// 경고
		LogWarning(Message);
	}
	else {
		// 정보성(LOW / NOTIFICATION)
		LogInfo(Message);
	}
}
