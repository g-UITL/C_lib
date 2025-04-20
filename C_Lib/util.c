
#include "util.h"

/*******pOut->value 동적할당하여 리턴하는 함수 외부에서 free를 꼭 해줘야함*******/
int readFile(const char* filePath, UString* pOut) {
    FILE* pFile = NULL;
    size_t nFileLen = 0;
    size_t nBytesRead = 0;
    errno_t err = 0;

    err = fopen_s(&pFile, filePath, "rb"); // 바이너리 읽기 모드로 파일 열기
    if (err != 0) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    //파일 크기 확인
    fseek(pFile, 0, SEEK_END); // 파일 끝으로 이동
    nFileLen = ftell(pFile); // 파일 크기 가져오기
    rewind(pFile);
    pOut->length = nFileLen;

    //메모리 할당
    pOut->value = (unsigned char*)calloc(nFileLen + 2, sizeof(unsigned char));
    if (pOut->value == NULL) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    //파일 읽기
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

    err = fopen_s(&pFile, filePath, "wb"); // 바이너리 쓰기 모드로 파일 열기
    if (err != 0) {
        perror("Failed to open output file");
        fclose(pFile);
        return EXIT_FAILURE;
    }

    // 버퍼를 사용하여 읽고 쓰기

    size_t bytesWritten = fwrite(pIn->value, 1, pIn->length, pFile);
    if (bytesWritten != pIn->length) {
        perror("fwrite");
        return EXIT_FAILURE;
    }

    fclose(pFile);
    printf("writeFile SUCEESS==\n");
    return EXIT_SUCCESS;
}