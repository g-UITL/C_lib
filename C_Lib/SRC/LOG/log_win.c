
#include "log_win.h"
#include "TYPE.h"
#include "util.h"
#include <Windows.h>

//전역변수(메모리)
#define BUFFER_SIZE 2048

static HANDLE g_hFile = INVALID_HANDLE_VALUE;
const char* g_logPath = "C:\\Temp\\GW_LOG.log";

static int GW_writeFile_win(const UString* pIn) {
    
    if (g_hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("GW_TRACE: 초기화 실패(g_hFile == INVALID_HANDLE_VALUE)");
        return EXIT_FAILURE;
    }

    char* pBuffer = NULL;
    unsigned int nLen = pIn->length + 20;
    pBuffer = (char*)calloc(nLen, sizeof(char));
    if (pBuffer == NULL)
    {
        OutputDebugStringA("calloc error");
        return EXIT_FAILURE;
    }
    
    strncpy_s(pBuffer, nLen, pIn->value, _TRUNCATE);

    //WriteFile함수 사용
    unsigned long bytesWritten = 0;
    BOOL result = WriteFile(
        g_hFile,           // 파일 핸들
        pBuffer,           // 쓸 데이터 버퍼
        (unsigned long)strlen(pBuffer),  // 쓸 바이트 수
        &bytesWritten,     // 실제로 쓰인 바이트 수
        NULL               // 중첩 구조체 없음
    );

    if (!result || bytesWritten != strlen(pBuffer)) {
        OutputDebugStringA("GW_writeFile failed: ");
        OutputDebugStringA(pIn->value);
        free(pBuffer);
        return EXIT_FAILURE;
    }

    // 파일을 즉시 디스크에 쓰기 (버퍼링 방지)
    FlushFileBuffers(g_hFile);


    if (pBuffer) free(pBuffer);

    return EXIT_SUCCESS;
}

/******************************************************************************
* 함 수 명 : GW_LogInit_win
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
int GW_LogInit_win(const char* pSavePath)
{
    FILE* pInitFile = NULL;
    errno_t err = 0;
    const char* pfilePath = pSavePath;
    if (pfilePath != NULL) {
        //인잣값으로 로그 저장경로를 받은경우
        goto no_config;
    }

    pfilePath = g_logPath; // 기본 로그 경로
    //로그 환경설정 파일 읽기
    err = fopen_s(&pInitFile, ".\\GW_Config.ini", "rb"); // 바이너리 쓰기 모드로 파일 열기
    if (err == 0) {
        OutputDebugStringA("GW_Config.ini open SUCCESS");
        printf("GW_Config.ini open SUCCESS");
        char szBuffer[1024];
        memset(szBuffer, 0x00, sizeof(szBuffer));
        size_t bytesWritten = fread(szBuffer, 1, 1023, pInitFile);
        char* pFind = strstr(szBuffer, "LOG_PATH");
        if (pFind == NULL) goto no_config;
        pFind += strlen("LOG_PATH");
        while (*pFind == '=' || *pFind == ' ') { pFind++; }
        char* pPath = pFind;
        for (int i = 0; pPath[i] != '\0'; i++) {
            if (isspace(pPath[i]))
            {
                pPath[i] = '\0';
                break;
            }
        }

        pfilePath = pPath;

        fclose(pInitFile);
    }
no_config:

    //이미 열린 파일이 있을경우 닫고 대체해주기
    if (g_hFile != INVALID_HANDLE_VALUE)
        GW_LogClose_win();

    //CreateFile 함수로 파일공유모드로 열기
    g_hFile = CreateFileA(
        pfilePath,				//파일 경로
        GENERIC_WRITE,			//쓰기 권한
        FILE_SHARE_READ,		//다른프로세스 읽기 허용
        NULL,					//보안 속성
        OPEN_ALWAYS,			//파일이 없으면 생성, 있으면 열기
        FILE_ATTRIBUTE_NORMAL,	//일반 파일 속성
        NULL					//템플릿파일 없음
    );

    if (g_hFile == INVALID_HANDLE_VALUE) {
        DWORD dError = GetLastError();
        char errMsg[2560];
        sprintf_s(errMsg, sizeof(errMsg), "GW_LogInit_win: 초기화 실패[%s](Failed to open output file) - Error code: %lu", pfilePath, dError);
        printf("%s\n", errMsg);
        OutputDebugStringA(errMsg);
        return EXIT_FAILURE;
    }

    printf("GW_LogInit_win 초기화 성공 [%s]\n", pfilePath);

    //파일 끝으로 포인터 이동(append 모드처럼 동작하게)
    SetFilePointer(g_hFile, 0, NULL, FILE_END);

    return EXIT_SUCCESS;
}

/******************************************************************************
* 함 수 명 : GW_LogTrace_win
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
int GW_LogTrace_win(const char* format, ...)
{
    UString usInput = { 0, 0 };
    char buffer[BUFFER_SIZE + 100]; //버퍼사이즈 + 시간 + 줄바꿈
    char bufferTmp[BUFFER_SIZE];    //버퍼사이즈
    char timeStr[32];               // 시간 문자열 저장
    va_list args;

    if (g_hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("GW_LogTrace_win: 초기화 실패(g_hFile == INVALID_HANDLE_VALUE)");
        printf("GW_LogTrace_win: 초기화 실패(g_hFile == INVALID_HANDLE_VALUE)");
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

	return GW_writeFile_win(&usInput);
   

}

/******************************************************************************
* 함 수 명 : GW_LogClose_win
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
int GW_LogClose_win()
{
    if (g_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
	return EXIT_SUCCESS;
}