
#include "log.h"
#include "TYPE.h"
#include "util.h"

//��������(�޸�)
FILE* g_pFile = NULL;
#define BUFFER_SIZE 2048

static int GW_writeFile(const UString* pIn) {
    
    //���۸� ��������(�޸�) ������ ����Ͽ� ����
    size_t bytesWritten = fwrite(pIn->value, 1, pIn->length, g_pFile);
    if (bytesWritten != pIn->length) {
        perror("fwrite");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/******************************************************************************
* �� �� �� : GW_LogInit
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
int GW_LogInit(const char* pSavePath)
{
    FILE* pFile = NULL;
    errno_t err = 0;

    err = fopen_s(&pFile, pSavePath, "ab"); // append + binary ���
    if (err != 0) {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    GW_LogClose(); //�̹� ���� ������ ������� �ݰ� ��ü���ֱ�

    g_pFile = pFile; //��������(�޸�)�� ������ ���� ����

	return EXIT_SUCCESS;
}

/******************************************************************************
* �� �� �� : GW_LogTrace
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
int GW_LogTrace(const char* format, ...)
{
    UString usInput = { 0, 0 };
    char buffer[BUFFER_SIZE + 100]; //���ۻ����� + �ð� + �ٹٲ�
    char bufferTmp[BUFFER_SIZE];    //���ۻ�����
    char timeStr[32];               // �ð� ���ڿ� ����
    va_list args;

    if (g_pFile == NULL)
    {
        //�ʱ�ȭ�� �ȵǾ� ����(���� ����)
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

	return GW_writeFile(&usInput);
    
#if 0 /* strncat_s ���� */
    errno_t strncat_s(
        char* dest,
        rsize_t destsz,
        const char* src,
        rsize_t count
    );


dest: ����� ������ ����
destsz : dest�� ��ü ũ��
src : ������ ���ڿ�
count : src���� ������ �ִ� ����(_TRUNCATE�� ���� �� �ڵ� �ڸ��� ����)

_TRUNCATE�� ���� �ʹ� �� ��� �ڵ����� �߶��ְ� �� ���ڵ� �������༭ �ſ� ������!
#endif /* strncat_s ���� */

}

/******************************************************************************
* �� �� �� : GW_LogClose
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
int GW_LogClose()
{
    if (g_pFile != NULL)
    {
        fclose(g_pFile);
        g_pFile = NULL;
    }
	return EXIT_SUCCESS;
}