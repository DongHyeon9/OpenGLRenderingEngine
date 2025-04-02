#include "Engine.h"

int main()
{
	WindowDesc desc{};
	if (!GEngine->Init(desc))
	{
		int a{};
		glfwTerminate();
		LOG("Engine Init Fail\nPlease Enter Key");
		std::cin >> a;

		return -1;
	}
	return GEngine->Run();
}