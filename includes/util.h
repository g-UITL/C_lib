#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TYPE.h"


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CLIB_EXPORTS
#define CLIB_API __declspec(dllexport)
#else
#define CLIB_API __declspec(dllimport)
#endif

/*******pOut->value 동적할당하여 리턴하는 함수 외부에서 free를 꼭 해줘야함*******/
CLIB_API int readFile(const char* filePath, UString* pOut);

//파일경로에 인풋값을 Write
CLIB_API int writeFile(const UString* pIn, const char* filePath);

#ifdef __cplusplus
}
#endif

#endif