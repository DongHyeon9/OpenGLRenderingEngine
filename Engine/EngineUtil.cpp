#include "EngineUtil.h"

GLuint EngineUtil::Shader::LoadVertexShader(const std::string& ShaderName)
{
	//���ؽ� ���̴� ��ü ����
	GLuint shaderID{ glCreateShader(GL_VERTEX_SHADER) };

	//���̴� ������ ��θ� ������ ���� ����, ���� ���� ������ code�� ����
	std::string shaderPath{ EngineUtil::Path::GetVertexShaderDir() + ShaderName + ShaderExtension::VERTEX.data() };
	std::string shaderCode{};
	std::ifstream shaderStream{ shaderPath.c_str(), std::ios::in};
	if (shaderStream.is_open())
	{
		std::stringstream sstr{};
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else
	{
		LOG("Impossible to open %s", ShaderName.c_str());
		return 0;
	}

	GLint result{ GL_FALSE };
	int infoLogLength{};

	LOG("Compiling Shader : %s", ShaderName.c_str());
	const char* shaderSourcePointer = shaderCode.c_str();
	//���̴� �ҽ��� ���̴� ��ü�� ����
	glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
	//���̴� ������
	glCompileShader(shaderID);

	//������ ����� �αױ��� Ȯ��
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::string shaderErrorMessage(static_cast<size_t>(infoLogLength + 1), 0);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderErrorMessage.data());
		LOG("Vertex Shader Error : %s", shaderErrorMessage.c_str());
		return 0;
	}

	return shaderID;
}

GLuint EngineUtil::Shader::LoadFragmentShader(const std::string& ShaderName)
{
	//�����׸�Ʈ ���̴� ��ü ����
	GLuint shaderID{ glCreateShader(GL_FRAGMENT_SHADER) };

	//���̴� ������ ��θ� ������ ���� ����, ���� ���� ������ code�� ����
	std::string shaderPath{ EngineUtil::Path::GetFragmentShaderDir() + ShaderName + ShaderExtension::FRAGMENT.data() };
	std::string shaderCode{};
	std::ifstream shaderStream{ shaderPath.c_str(), std::ios::in };
	if (shaderStream.is_open())
	{
		std::stringstream sstr{};
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else
	{
		LOG("Impossible to open %s", ShaderName.c_str());
		return 0;
	}

	GLint result{ GL_FALSE };
	int infoLogLength{};

	LOG("Compiling Shader : %s", ShaderName.c_str());
	const char* shaderSourcePointer = shaderCode.c_str();
	//���̴� �ҽ��� ���̴� ��ü�� ����
	glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
	//���̴� ������
	glCompileShader(shaderID);

	//������ ����� �αױ��� Ȯ��
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::string shaderErrorMessage(static_cast<size_t>(infoLogLength + 1), 0);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, shaderErrorMessage.data());
		LOG("Fragment Shader Error : %s", shaderErrorMessage.c_str());
		return 0;
	}

	return shaderID;
}

std::string EngineUtil::Path::PrevDir(std::string Path, size_t Count)
{
	std::string result{ std::move(Path) };

	if (!result.empty() && result.back() == '\\')
		result.pop_back();

	for (size_t i = 0; i < Count; ++i)
	{
		while (!result.empty() && result.back() != '\\')
			result.pop_back();
	}

	return result;
}

std::string EngineUtil::Path::GetCurrentDir()
{
	return std::filesystem::current_path().string() + '\\';
}

std::string EngineUtil::Path::GetResourceDir()
{
	return AddDir(PrevDir(GetCurrentDir()), "Res");
}

std::string EngineUtil::Path::GetEngineDir()
{
	return AddDir(PrevDir(GetCurrentDir()), "Engine");
}

std::string EngineUtil::Path::GetVertexShaderDir()
{
	return AddDir(GetEngineDir(), "Shader", "Vertex");
}

std::string EngineUtil::Path::GetFragmentShaderDir()
{
	return AddDir(GetEngineDir(), "Shader", "Fragment");
}

std::string EngineUtil::Path::GetTextureDir()
{
	return AddDir(GetResourceDir(), "Texture");
}

std::string EngineUtil::Path::GetMeshDir()
{
	return AddDir(GetResourceDir(), "Mesh");
}

bool EngineUtil::OpenGL::CheckProgram(GLuint ProgramID)
{
	GLint result{ GL_FALSE };
	int infoLogLength{};
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(ProgramID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		LOG("Program Error : %s\n", &ProgramErrorMessage[0]);
		return false;
	}
	return true;
}
