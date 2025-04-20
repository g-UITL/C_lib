#ifndef _SHA3_H_
#define _SHA3_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" 
{
#endif

#ifdef CLIB_EXPORTS
	#define CLIB_API __declspec(dllexport)
#else
	#define CLIB_API __declspec(dllimport)
#endif
	   
CLIB_API int sha3_hash(uint8_t *output, int outLen, uint8_t *input, int inLen, int bitSize, int useSHAKE);

#ifdef __cplusplus
}
#endif

#endif
