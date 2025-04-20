
#include "util.h"

/*******pOut->value �����Ҵ��Ͽ� �����ϴ� �Լ� �ܺο��� free�� �� �������*******/
int readFile(const char* filePath, UString* pOut) {
    FILE* pFile = NULL;
    size_t nFileLen = 0;
    size_t nBytesRead = 0;
    errno_t err = 0;

    err = fopen_s(&pFile, filePath, "rb"); // ���̳ʸ� �б� ���� ���� ����
    if (err != 0) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    //���� ũ�� Ȯ��
    fseek(pFile, 0, SEEK_END); // ���� ������ �̵�
    nFileLen = ftell(pFile); // ���� ũ�� ��������
    rewind(pFile);
    pOut->length = nFileLen;

    //�޸� �Ҵ�
    pOut->value = (unsigned char*)calloc(nFileLen + 2, sizeof(unsigned char));
    if (pOut->value == NULL) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    //���� �б�
    nBytesRead = fread(pOut->value, sizeof(unsigned char), nFileLen, pFile);
    if (nBytesRead != nFileLen) {
        perror("fread");
        return EXIT_FAILURE;
    }
    pOut->value[nFileLen] = '\0';


    fclose(pFile);
    return EXIT_SUCCESS;
}

int writeFile(const UString* pIn, const char* filePath) {
    //const char* filePath = "C:\\Users\\user\\source\\repos\\01_UTIL\\ConsolTest\\x64\\Release\\test.exe";
    FILE* pFile = NULL;
    errno_t err = 0;

    err = fopen_s(&pFile, filePath, "wb"); // ���̳ʸ� ���� ���� ���� ����
    if (err != 0) {
        perror("Failed to open output file");
        fclose(pFile);
        return EXIT_FAILURE;
    }

    // ���۸� ����Ͽ� �а� ����

    size_t bytesWritten = fwrite(pIn->value, 1, pIn->length, pFile);
    if (bytesWritten != pIn->length) {
        perror("fwrite");
        return EXIT_FAILURE;
    }

    fclose(pFile);
    printf("writeFile SUCEESS==\n");
    return EXIT_SUCCESS;
}