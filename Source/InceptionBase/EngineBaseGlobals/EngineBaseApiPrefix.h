#pragma once
#ifdef WIN32
#ifdef InceptionBase_EXPORTS
#define IE_Base_API __declspec(dllexport)
#else
#define IE_Base_API __declspec(dllimport)
#endif
#elif defined __linux__
#define IE_Base_API
#endif
