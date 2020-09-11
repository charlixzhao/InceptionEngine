#pragma once

#ifdef InceptionEngine_EXPORTS
#define IE_API __declspec(dllexport)
#else
#define IE_API __declspec(dllimport)
#endif