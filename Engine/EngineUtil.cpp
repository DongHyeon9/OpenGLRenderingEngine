#include "EngineUtil.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint EngineUtil::Shader::LoadVertexShader(const std::string& ShaderName)
{
	//버텍스 쉐이더 객체 생성
	GLuint shaderID{ glCreateShader(GL_VERTEX_SHADER) };

	//쉐이더 파일의 경로를 가져와 파일 오픈, 이후 파일 내부의 code를 저장
	std::string shaderPath{ EngineUtil::Path::GetVertexShaderDir() + ShaderName + SHADER_EXTENSION::VERTEX.data() };
	std::string shaderCode{};
	std::ifstream shaderStream{ shaderPath, std::ios::in};
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
	//쉐이더 소스를 쉐이더 객체에 연결
	glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
	//쉐이더 컴파일
	glCompileShader(shaderID);

	//컴파일 결과와 로그길이 확인
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
	//프래그먼트 쉐이더 객체 생성
	GLuint shaderID{ glCreateShader(GL_FRAGMENT_SHADER) };

	//쉐이더 파일의 경로를 가져와 파일 오픈, 이후 파일 내부의 code를 저장
	std::string shaderPath{ EngineUtil::Path::GetFragmentShaderDir() + ShaderName + SHADER_EXTENSION::FRAGMENT.data() };
	std::string shaderCode{};
	std::ifstream shaderStream{ shaderPath, std::ios::in };
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
	//쉐이더 소스를 쉐이더 객체에 연결
	glShaderSource(shaderID, 1, &shaderSourcePointer, NULL);
	//쉐이더 컴파일
	glCompileShader(shaderID);

	//컴파일 결과와 로그길이 확인
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

GLuint EngineUtil::Shader::LoadTexture(const std::string& TextureName)
{
	std::string texturePath{ EngineUtil::Path::GetTextureDir() + TextureName };

	// 파일 확장자 추출 (소문자로 변환하여 비교)
	std::string extension{};
	auto pos = texturePath.find_last_of('.');
	if (pos != std::string::npos) {
		extension = texturePath.substr(pos + 1);
		// 간단한 소문자 변환
		for (auto& c : extension) c = std::tolower(c);
	}

	if (extension == "dds") {
		std::ifstream file(texturePath, std::ios::binary);
		CHECK(file.is_open(), "Can't Open File : %s", 0, TextureName.c_str());

		char filecode[4]{};
		file.read(filecode, 4);
		CHECK(std::strncmp(filecode, "DDS ", 4) == 0, "Isn't Valid DDS File", 0);

		char header[124]{};
		file.read(header, 124);

		// 헤더에서 필요한 정보 추출 (리틀 엔디언 가정)
		unsigned int height = *reinterpret_cast<unsigned int*>(&header[8]);
		unsigned int width = *reinterpret_cast<unsigned int*>(&header[12]);
		unsigned int linearSize = *reinterpret_cast<unsigned int*>(&header[16]);
		unsigned int mipMapCount = *reinterpret_cast<unsigned int*>(&header[24]);
		unsigned int fourCC = *reinterpret_cast<unsigned int*>(&header[80]);

		// 전체 텍스처 데이터를 저장할 버퍼 크기 결정
		unsigned int bufsize = (mipMapCount > 1) ? linearSize * 2 : linearSize;
		std::vector<unsigned char> buffer(bufsize);
		file.read(reinterpret_cast<char*>(buffer.data()), bufsize);
		file.close();

		// 압축 텍스처의 컴포넌트 수 결정 (DXT1은 3, 그 외는 4)
		unsigned int components = (fourCC == FOURCC_DXT1) ? 3U : 4U;
		unsigned int format{};
		switch (fourCC) {
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default: {
			std::cerr << "Unsupported DDS format" << std::endl;
			return 0;
		}
		}

		// OpenGL 텍스처 생성
		GLuint textureID{};
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // 데이터 정렬 설정

		// 블록 크기 결정 (DXT1은 8바이트, DXT3/DXT5는 16바이트)
		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8U : 16U;
		unsigned int offset = 0;

		// 각 MIP 레벨에 대해 압축 텍스처 데이터 업로드
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
			unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer.data() + offset);

			offset += size;
			width = (width > 1 ? width / 2 : 1);
			height = (height > 1 ? height / 2 : 1);
		}
		return textureID;
	}
	// DDS가 아닌 경우: BMP, PNG 등 stb_image 지원 포맷 사용
	else {
		int width, height, channels;
		// stbi_load는 좌우반전 등의 옵션이 필요하면 stbi_set_flip_vertically_on_load(true); 를 호출할 수 있음
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
		CHECK(data != nullptr, "Failed to load texture : %s", 0, TextureName.c_str());

		GLuint textureID{};
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// 이미지의 채널 수에 따른 포맷 결정
		GLenum format;
		if (channels == 1)
			format = GL_RED;
		else if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;
		else {
			stbi_image_free(data);
			std::cerr << "Unsupported image channel count: " << channels << std::endl;
			return 0;
		}

		// 텍스처 데이터 업로드
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 텍스처 래핑 및 필터링 옵션 설정 (필요에 따라 조정)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		return textureID;
	}
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
