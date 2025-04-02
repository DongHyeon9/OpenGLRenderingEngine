#include "Engine.h"

std::unique_ptr<Engine> GEngine{ std::make_unique<Engine>() };

bool Engine::Init(WindowDesc Desc)
{
	winDesc = std::move(Desc);

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

	//최신 OpenGL 기능을 사용할 수 있도록 실험적 기능을 활성화
	glewExperimental = true;
	//GLEW를 초기화, OpenGL 확장 기능들을 사용
	CHECK(glewInit() == GLEW_OK, "Failed to initialize GLEW", false);

	//키 입력이 발생하면 해당 키의 상태가 일정 기간 유지되도록 하여, 폴링 시 놓치는 입력이 없게함
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//키 입력 시 호출될 콜백 함수 등록
	glfwSetKeyCallback(window, OpenGLWndProc);

	//기본 배경색 설정
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
	//버텍스 버퍼 객체(VBO)를 생성, ID할당
	glGenBuffers(
		1,				//생성할 버퍼의 개수
		&vertexbuffer);	//생성된 버퍼 ID를 저장

	//생성된 버퍼를 활성화, 이후 버퍼 관련 호출에서 이 버퍼를 대상으로 동작하도록 만듬
	glBindBuffer(GL_ARRAY_BUFFER,		//버퍼 타입, 정점 데이터를 저장
		vertexbuffer);					//버퍼 객체의 ID
	//정점 데이터를 GPU 메모리로 복사
	glBufferData(GL_ARRAY_BUFFER,	//전송될 버퍼 타입
		sizeof(vectexBufferData),	//전송할 데이터의 크기(바이트 단위)
		vectexBufferData,			//전송할 실제 데이터
		GL_STATIC_DRAW);			//데이터 사용 패턴(데이터가 변경되지 않고 GPU에 저장되어 렌더링 시 반복 사용됨)

	do {
		//프레임 버퍼의 색상 정보 초기화
		glClear(GL_COLOR_BUFFER_BIT);

		//미리 컴파일되고 링크된 쉐이더 프로그램 활성화
		glUseProgram(programID);

		//인덱스 0번의 정점 속성 배열을 활성화
		glEnableVertexAttribArray(0);
		//정점 버퍼 객체를 다시 바인딩
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//정점 데이터의 속성 레이아웃을 지정
		glVertexAttribPointer(
			0,			//쉐이더에서 참조할 정점 속성의 위치
			3,			//각 정점에 대해 3개의 요소(x, y, z)가 사용
			GL_FLOAT,	//각 요소의 자료형이 float임
			GL_FALSE,	//데이터를 정규화 여부(그대로 사용)
			0,			//각 정점 사이의 간격(0이면 데이터가 연속됨)
			(void*)0	//버퍼 내에서 데이터 시작 위치(0바이트 오프셋)
		);

		//버텍스 배열에서 0번 인덱스부터 시작해 3개의 버텍스를 사용하여 삼각형을 그림
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//인덱스 0번의 정점 속성 배열을 비활성화, 이후 렌더링에 영향을 주지 않도록 설정
		glDisableVertexAttribArray(0);

		//백 버퍼와 프론트 버퍼를 교체
		glfwSwapBuffers(window);
		//GLFW 이벤트(키 입력, 마우스 움직임 등)를 처리
		glfwPollEvents();

	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&		//ESC 키가 눌리지 않은 경우
		glfwWindowShouldClose(window) == 0);						//창이 닫히지 않은 경우

	//GPU 메모리에서 사용한 버퍼 객체를 해제
	glDeleteBuffers(1,	//삭제할 버퍼의 개수
		&vertexbuffer);	//삭제할 버퍼 객체의 주소
	ShutDown();

	return 0;
}

void Engine::WndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{

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

	GLuint vertexShaderID{ EngineUtil::Shader::LoadVertexShader("SimpleVertexShader") };
	GLuint fragmentShaderID{ EngineUtil::Shader::LoadFragmentShader("SimpleFragmentShader") };

	//생성된 프로그램 객체에 쉐이더 첨부
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//첨부된 쉐이더들을 링크하여 최종 실행 가능한 쉐이더 프로그램 생성
	glLinkProgram(programID);

	return EngineUtil::OpenGL::CheckProgram(programID);
}

void Engine::ShutDown()
{
	//생성된 VAO를 삭제
	glDeleteVertexArrays(1,		//삭제할 VAO의 개수
		&vertexArrayID);		//삭제할 VAO의 ID
	//생성된 쉐이더 프로그램 객체를 삭제
	glDeleteProgram(programID);

	//GLFW 라이브러리의 종료 작업을 수행하여, 창, 이벤트 및 기타 관련 리소스를 모두 정리
	glfwTerminate();
}

void OpenGLWndProc(GLFWwindow* Window, int Key, int ScanCode, int Action, int Mods)
{
	GEngine->WndProc(Window, Key, ScanCode, Action, Mods);
}
