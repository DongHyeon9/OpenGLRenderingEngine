#pragma once

#ifdef _DEBUG

#define LOG(str, ...) printf_s(str, ##__VA_ARGS__);std::cout <<std::endl;

#else
#define LOG_LINE ((void)0)
#define LOG(str,...) ((void)0)
#endif	//_DEBUG

#define CHECK(exp,str,ret)		\
if(!(exp))	{					\
	LOG(str);					\
	return ret;					\
}