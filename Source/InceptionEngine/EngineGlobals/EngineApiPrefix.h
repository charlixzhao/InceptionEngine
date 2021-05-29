#pragma once
#ifdef WIN32
#ifdef InceptionEngine_EXPORTS
#define IE_API __declspec(dllexport)
#else
#define IE_API __declspec(dllimport)
#endif
#else
#define IE_API
#endif
