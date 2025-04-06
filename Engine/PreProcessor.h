#pragma once

#define FOURCC_DXT1 0x31545844 // "DXT1"
#define FOURCC_DXT3 0x33545844 // "DXT3"
#define FOURCC_DXT5 0x35545844 // "DXT5"

#define KEY_DOWN(key) InputManager::GetInstance()->GetButtonDown(key)
#define KEY_PRESS(key) InputManager::GetInstance()->GetButtonPress(key)
#define KEY_UP(key) InputManager::GetInstance()->GetButtonUp(key)

#define DECLARE_SINGLETON(type)				\
private:									\
	type() { }								\
public:										\
	static type* GetInstance()				\
	{										\
		static type singleinst;				\
		return &singleinst;					\
	}

#ifdef _DEBUG

#define LOG(str, ...) printf_s(str, ##__VA_ARGS__);std::cout <<std::endl;

#else
#define LOG_LINE ((void)0)
#define LOG(str,...) ((void)0)
#endif	//_DEBUG

#define CHECK(exp,str,ret, ...)	\
if(!(exp))	{					\
	LOG(str,##__VA_ARGS__);		\
	return ret;					\
}