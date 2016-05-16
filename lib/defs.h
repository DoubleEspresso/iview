#ifndef LIB_DEFS_H
#define LIB_DEFS_H

#define M_PI 3.14159265358979323846264338327950288

#ifdef _WIN32
	#define ImageLibWIN32_API __declspec(dllexport)
	#define LIB lib
	#define DECL __cdecl
#else
	#define ImageLibWIN32_API
	#define LIB
	#define DECL
#endif


#endif