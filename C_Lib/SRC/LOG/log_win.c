
#include "log_win.h"
#include "TYPE.h"
#include "util.h"
#include <Windows.h>

//��������(�޸�)
#define BUFFER_SIZE 2048

static HANDLE g_hFile = INVALID_HANDLE_VALUE;
const char* g_logPath = "C:\\Temp\\GW_LOG.log";

static int GW_writeFile_win(const UString* pIn) {
    
    if (g_hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("GW_TRACE: �ʱ�ȭ ����(g_hFile == INVALID_HANDLE_VALUE)");
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

    //WriteFile�Լ� ���
    unsigned long bytesWritten = 0;
    BOOL result = WriteFile(
        g_hFile,           // ���� �ڵ�
        pBuffer,           // �� ������ ����
        (unsigned long)strlen(pBuffer),  // �� ����Ʈ ��
        &bytesWritten,     // ������ ���� ����Ʈ ��
        NULL               // ��ø ����ü ����
    );

    if (!result || bytesWritten != strlen(pBuffer)) {
        OutputDebugStringA("GW_writeFile failed: ");
        OutputDebugStringA(pIn->value);
        free(pBuffer);
        return EXIT_FAILURE;
    }

    // ������ ��� ��ũ�� ���� (���۸� ����)
    FlushFileBuffers(g_hFile);


    if (pBuffer) free(pBuffer);

    return EXIT_SUCCESS;
}

/******************************************************************************
* �� �� �� : GW_LogInit_win
*------------------------------------------------------------------------------
* �Է����� :
*		const char*     pSavePath	- ���� ���
*
* ������� :
*
* �� �� �� :
*       ����    - EXIT_SUCCESS (0)
*       ����    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* ��    �� : �α׸� ���� ��θ� �ʱ�ȭ ���ش�.
*------------------------------------------------------------------------------
* ��	�� : ��������(�޸�)�� FILE* �� ����(g_pFile)
*       ���Ͽ��� append ��� fopen_s(&pFile, pSavePath, "ab")
******************************************************************************/
int GW_LogInit_win(const char* pSavePath)
{
    FILE* pInitFile = NULL;
    errno_t err = 0;
    const char* pfilePath = pSavePath;
    if (pfilePath != NULL) {
        //���㰪���� �α� �����θ� �������
        goto no_config;
    }

    pfilePath = g_logPath; // �⺻ �α� ���
    //�α� ȯ�漳�� ���� �б�
    err = fopen_s(&pInitFile, ".\\GW_Config.ini", "rb"); // ���̳ʸ� ���� ���� ���� ����
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

    //�̹� ���� ������ ������� �ݰ� ��ü���ֱ�
    if (g_hFile != INVALID_HANDLE_VALUE)
        GW_LogClose_win();

    //CreateFile �Լ��� ���ϰ������� ����
    g_hFile = CreateFileA(
        pfilePath,				//���� ���
        GENERIC_WRITE,			//���� ����
        FILE_SHARE_READ,		//�ٸ����μ��� �б� ���
        NULL,					//���� �Ӽ�
        OPEN_ALWAYS,			//������ ������ ����, ������ ����
        FILE_ATTRIBUTE_NORMAL,	//�Ϲ� ���� �Ӽ�
        NULL					//���ø����� ����
    );

    if (g_hFile == INVALID_HANDLE_VALUE) {
        DWORD dError = GetLastError();
        char errMsg[2560];
        sprintf_s(errMsg, sizeof(errMsg), "GW_LogInit_win: �ʱ�ȭ ����[%s](Failed to open output file) - Error code: %lu", pfilePath, dError);
        printf("%s\n", errMsg);
        OutputDebugStringA(errMsg);
        return EXIT_FAILURE;
    }

    printf("GW_LogInit_win �ʱ�ȭ ���� [%s]\n", pfilePath);

    //���� ������ ������ �̵�(append ���ó�� �����ϰ�)
    SetFilePointer(g_hFile, 0, NULL, FILE_END);

    return EXIT_SUCCESS;
}

/******************************************************************************
* �� �� �� : GW_LogTrace_win
*------------------------------------------------------------------------------
* �Է����� :
*		const char*        format, ...	- printf ������ ����
*
* ������� :
*
* �� �� �� :
*       ����    - EXIT_SUCCESS (0)
*       ����    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* ��    �� : �α׸� �����Ѵ�.
*------------------------------------------------------------------------------
* ��	�� : ��������(�޸�)�� FILE*�� �α׸� ���� ���ο��� GW_writeFile ȣ��
******************************************************************************/
int GW_LogTrace_win(const char* format, ...)
{
    UString usInput = { 0, 0 };
    char buffer[BUFFER_SIZE + 100]; //���ۻ����� + �ð� + �ٹٲ�
    char bufferTmp[BUFFER_SIZE];    //���ۻ�����
    char timeStr[32];               // �ð� ���ڿ� ����
    va_list args;

    if (g_hFile == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("GW_LogTrace_win: �ʱ�ȭ ����(g_hFile == INVALID_HANDLE_VALUE)");
        printf("GW_LogTrace_win: �ʱ�ȭ ����(g_hFile == INVALID_HANDLE_VALUE)");
        return EXIT_FAILURE;
    }

    //1.�ʱ�ȭ
    memset(buffer, 0x00, sizeof(buffer));
    memset(bufferTmp, 0x00, sizeof(bufferTmp));
    memset(timeStr, 0x00, sizeof(timeStr));

    //2.�ð� �߰�
    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", ptm);
    strncat_s(buffer, sizeof(buffer), timeStr, _TRUNCATE);  // �ð� ���ڿ� ���� ���̱�
    
    //3.���� ��������
    va_start(args, format);
    vsnprintf(bufferTmp, sizeof(bufferTmp), format, args);
    va_end(args);
    strncat_s(buffer, sizeof(buffer), bufferTmp, _TRUNCATE);

    //4.�ٹٲ� �߰�
    strncat_s(buffer, sizeof(buffer), "\r\n", _TRUNCATE);

    usInput.value = buffer;
    usInput.length = strlen(buffer);

	return GW_writeFile_win(&usInput);
   

}

/******************************************************************************
* �� �� �� : GW_LogClose_win
*------------------------------------------------------------------------------
* �Է����� :
*
* ������� :
*
* �� �� �� :
*       ����    - EXIT_SUCCESS (0)
*       ����    - EXIT_FAILURE (1)
*------------------------------------------------------------------------------
* ��    �� : File��  fclose�� �ݾ��ش�.
*------------------------------------------------------------------------------
* ��	�� : ��������(�޸�)�� FILE*�� �ݾ��ش�.
******************************************************************************/
int GW_LogClose_win()
{
    if (g_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
	return EXIT_SUCCESS;
}