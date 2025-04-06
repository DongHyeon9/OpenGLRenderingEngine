#pragma once
#include "Value.h"
#include "PreProcessor.h"

namespace EngineUtil
{
	namespace Shader
	{
		GLuint LoadVertexShader(const std::string& ShaderName);
		GLuint LoadFragmentShader(const std::string& ShaderName);
		GLuint LoadTexture(const std::string& TextureName);
	}

	namespace OpenGL
	{
		bool CheckProgram(GLuint ProgramID);
	}

	namespace Path
	{
		template<class _Src>
		std::string AddDir(const std::string& MainPath, const _Src& SubPath)
		{
			std::string result = MainPath;
			if (result.back() != '\\')
				result += '\\';
			result += SubPath;
			result += '\\';
			return result;
		}

		template<class _Src, class ...Types>
		std::string AddDir(const std::string& MainPath, const _Src& SubPath, Types ...Args)
		{
			std::string result = MainPath;
			if (result.back() != '\\')
				result += '\\';
			result += SubPath;
			return AddDir(result, Args...);
		}

		std::string PrevDir(std::string Path, size_t Count = 1);
		std::string GetCurrentDir();
		std::string GetResourceDir();

		std::string GetEngineDir();
		std::string GetVertexShaderDir();
		std::string GetFragmentShaderDir();

		std::string GetTextureDir();
		std::string GetMeshDir();
	}
}

namespace std
{
	template<>
	struct hash<E_KEY_TYPE>
	{
		size_t operator()(E_KEY_TYPE Wrapper) const
		{
			return std::hash<int>{}(static_cast<uint16>(Wrapper));
		}
	};
}