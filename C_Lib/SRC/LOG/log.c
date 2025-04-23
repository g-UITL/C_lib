
#include "log.h"
#include "TYPE.h"
#include "util.h"

//전역변수(메모리)
FILE* g_pFile = NULL;
#define BUFFER_SIZE 2048

static int GW_writeFile(const UString* pIn) {
    
    //버퍼를 전역변수(메모리) 파일을 사용하여 쓰기
    size_t bytesWritten = fwrite(pIn->value, 1, pIn->length, g_pFile);
    if (bytesWritten != pIn->length) {
        perror("fwrite");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/******************************************************************************
* 함 수 명 : GW_LogInit
*------------------------------------------------------------------------------
* 입력인자 :
*		const char*     pSavePath	- 저장 경로
*
* 출력인자 :
*
* 리 턴 값 :
*       성공    - EXIT_SUCCESS (0)
*       실패    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* 기    능 : 로그를 저장 경로를 초기화 해준다.
*------------------------------------------------------------------------------
* 비	고 : 전역변수(메모리)로 FILE* 를 관리(g_pFile)
*       파일오픈 append 모드 fopen_s(&pFile, pSavePath, "ab")
******************************************************************************/
int GW_LogInit(const char* pSavePath)
{
    FILE* pFile = NULL;
    errno_t err = 0;

    err = fopen_s(&pFile, pSavePath, "ab"); // append + binary 모드
    if (err != 0) {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    GW_LogClose(); //이미 열린 파일이 있을경우 닫고 대체해주기

    g_pFile = pFile; //전역변수(메모리)에 오픈한 파일 삽입

	return EXIT_SUCCESS;
}

/******************************************************************************
* 함 수 명 : GW_LogTrace
*------------------------------------------------------------------------------
* 입력인자 :
*		const char*        format, ...	- printf 형태의 포맷
*
* 출력인자 :
*
* 리 턴 값 :
*       성공    - EXIT_SUCCESS (0)
*       실패    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* 기    능 : 로그를 저장한다.
*------------------------------------------------------------------------------
* 비	고 : 전역변수(메모리)로 FILE*에 로그를 저장 내부에서 GW_writeFile 호출
******************************************************************************/
int GW_LogTrace(const char* format, ...)
{
    UString usInput = { 0, 0 };
    char buffer[BUFFER_SIZE + 100]; //버퍼사이즈 + 시간 + 줄바꿈
    char bufferTmp[BUFFER_SIZE];    //버퍼사이즈
    char timeStr[32];               // 시간 문자열 저장
    va_list args;

    if (g_pFile == NULL)
    {
        //초기화가 안되어 있음(파일 오픈)
        return EXIT_FAILURE;
    }

    //1.초기화
    memset(buffer, 0x00, sizeof(buffer));
    memset(bufferTmp, 0x00, sizeof(bufferTmp));
    memset(timeStr, 0x00, sizeof(timeStr));

    //2.시간 추가
    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", ptm);
    strncat_s(buffer, sizeof(buffer), timeStr, _TRUNCATE);  // 시간 문자열 먼저 붙이기
    
    //3.포맷 가져오기
    va_start(args, format);
    vsnprintf(bufferTmp, sizeof(bufferTmp), format, args);
    va_end(args);
    strncat_s(buffer, sizeof(buffer), bufferTmp, _TRUNCATE);

    //4.줄바꿈 추가
    strncat_s(buffer, sizeof(buffer), "\r\n", _TRUNCATE);

    usInput.value = buffer;
    usInput.length = strlen(buffer);

	return GW_writeFile(&usInput);
    
#if 0 /* strncat_s 설명 */
    errno_t strncat_s(
        char* dest,
        rsize_t destsz,
        const char* src,
        rsize_t count
    );


dest: 결과를 저장할 버퍼
destsz : dest의 전체 크기
src : 연결할 문자열
count : src에서 복사할 최대 길이(_TRUNCATE로 지정 시 자동 자르기 가능)

_TRUNCATE를 쓰면 너무 길 경우 자동으로 잘라주고 널 문자도 보장해줘서 매우 안전해!
#endif /* strncat_s 설명 */

}

/******************************************************************************
* 함 수 명 : GW_LogClose
*------------------------------------------------------------------------------
* 입력인자 :
*
* 출력인자 :
*
* 리 턴 값 :
*       성공    - EXIT_SUCCESS (0)
*       실패    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* 기    능 : File을  fclose로 닫아준다.
*------------------------------------------------------------------------------
* 비	고 : 전역변수(메모리)로 FILE*을 닫아준다.
******************************************************************************/
int GW_LogClose()
{
    if (g_pFile != NULL)
    {
        fclose(g_pFile);
        g_pFile = NULL;
    }
	return EXIT_SUCCESS;
}