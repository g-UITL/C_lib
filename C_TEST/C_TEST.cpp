// C_TEST.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <string>
#include "KISA_SHA256.h"
#include "sha3.h"
#include "TYPE.h"
#include "util.h"

int sha3_test(const char *pInput)
{
	uint8_t out[512] = { 0, };
	//uint8_t in[200] = { 0, };
	uint8_t* in = NULL;

	int out_length = 0;		//byte size
	int in_length = 200;	//byte size
	int hash_bit = 0;		//bit(224,256,384,512)
	int SHAKE = 0;			//0 or 1

	int i, result;

	//memset(in, 0xA3, 200);
	in = (uint8_t*)pInput;
	in_length = strlen(pInput);

	printf("* SHA-3 test *\n\n");
	//printf("test message : A3(x200)\n\n");

	/* non-SHAKE test */
	SHAKE = 0;

	/* SHA3-224 test */
	out_length = 224 / 8;
	hash_bit = 224;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHA3-224 test\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");

	/* SHA3-256 test */
	out_length = 256 / 8;
	hash_bit = 256;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHA3-256 test\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");

	/* SHA3-384 test */
	out_length = 384 / 8;
	hash_bit = 384;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHA3-384 test\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");

	/* SHA3-512 test */
	out_length = 512 / 8;
	hash_bit = 512;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHA3-512 test\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");


	/* SHAKE test */
	SHAKE = 1;

	/* SHAKE128 test */
	out_length = 512;
	hash_bit = 128;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHAKE256 test\n");
	printf("output : 512bytes\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");

	/* SHAKE256 test */
	out_length = 512;
	hash_bit = 256;
	result = sha3_hash(out, out_length, in, in_length, hash_bit, SHAKE);

	printf("SHAKE256 test\n");
	printf("output : 512bytes\n");
	printf("hash : ");
	for (i = 0; i < out_length; i++)
		printf("%02X ", out[i]);
	printf("\n\n");

    return 0;
}

int sha256_test(const char *pInput)
{
	uint8_t out[33] = { 0, };
	UString usInput = { 0, 0 };
	//const char* pInput = "hello";
	const int nInputLen = strlen(pInput);
	const int out_length = 32; // Sha-256 -> 32바이트

	usInput.value = (unsigned char*)calloc(200, sizeof(unsigned char));
	if (usInput.value == NULL) {
		return -1;
	}

	memcpy(usInput.value, pInput, nInputLen);
	usInput.length = nInputLen;

	printf("* SHA-256 test *\n\n");
//	printf("test message : %s[%d]\n\n", pInput, nInputLen);

	SHA256_Encrpyt(usInput.value, usInput.length, out);

	printf("SHA-256 test\n");
	printf("hash : ");
	for (int i = 0; i < out_length; i++)
		printf("%02X", out[i]);
	printf("\n\n");
    return 0;
}

void prompt()
{
	std::cout << "[1]: Sha-256 테스트\n";
	std::cout << "[2]: Sha-3 테스트\n";
	std::cout << "[3]: ReadFile 테스트\n";
	std::cout << "[4]: WriteFile 테스트\n";
	std::cout << "[q]: 프로그램 종료\n";
	std::cout << "테스트 번호를 입력하시오: ";
}

void HashTest(int index)
{
	std::string strInput;
	std::cout << "원문값을 입력하시오(default=test): ";
	std::cin.ignore();
	std::getline(std::cin, strInput);

	if (strInput.empty()) 
	{
		strInput = "test";
	}

	std::cout << "원문값: " << strInput << std::endl;
	std::cout << "\n\n";

	switch (index)
	{
	case 1:
		std::cout << "[1]: Sha-256 테스트\n";
		sha256_test(strInput.c_str());
		break;
	case 2:
		std::cout << "[2]: Sha-3 테스트\n";
		sha3_test(strInput.c_str());
		break;
	}

	return;
}

#include <io.h>

int readFileTest(const char* pPath)
{
	UString usOut = { 0, 0 };
	if (readFile(pPath, &usOut) != EXIT_SUCCESS)
	{
		return EXIT_FAILURE;
	}

	std::cout << "파일경로: " << pPath << "\n";

	//usOut.value[usOut.length] = '\0';
	//_write(1, usOut.value, usOut.length);
	//printf("printf 파일내용[%s]\n", usOut.value);
	std::cout << "파일내용[" << usOut.value << "]\n";
	return 0;
}

int writeFileTest(const char* pPath)
{
	std::string strInput;
	std::cout << "입력할 내용을 입력하시오(default=testInput 123 안녕하세요): ";
	std::cin.ignore();
	std::getline(std::cin, strInput);

	if (strInput.empty())
	{
		strInput = "testInput 123 안녕하세요";
	}

	UString usIn = { 0, 0 };
	usIn.value = (unsigned char*)strInput.c_str();
	usIn.length = strInput.size();
	if (writeFile(&usIn, pPath) != EXIT_SUCCESS)
	{
		std::cerr << "writeFile FAIL" << "\n";
		return EXIT_FAILURE;
	}

	std::cout << "writeFile SUCCESS 파일경로: " << pPath << "\n";
	return 0;
}

void ReadWriteTest(int index)
{
	std::string strInput;
	std::cout << "파일경로를 입력하시오(default=C:\\Temp): ";
	std::cin.ignore();
	std::getline(std::cin, strInput);

	if (strInput.empty())
	{
		strInput = "C:\\Temp";
	}

	std::cout << "파일경로: " << strInput << std::endl;
	std::cout << "\n\n";

	switch (index)
	{
	case 3:
		std::cout << "[3]: ReadFile 테스트\n";
		readFileTest(strInput.c_str());
		break;
	case 4:
		std::cout << "[4]: WriteFile 테스트\n";
		writeFileTest(strInput.c_str());
		break;
	}

}

int main()
{
	while (1)
	{
		prompt();
		std::cout << "\n\n";
		std::string strInput;
		std::cin >> strInput;

		if (strInput == "q")
			break;

		int nInput = atoi(strInput.c_str());
		switch (nInput) {
		case 1:
		case 2:
			std::cout << "해시 테스트\n";
			HashTest(nInput);
			break;
		case 3:
		case 4:
			std::cout << "파일 입출력 테스트\n";
			ReadWriteTest(nInput);
			break;
		default:
			std::cout << "잘못된 입력입니다." << std::endl;
			break;
		}

		std::cout << "\n\n";
	}

    return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
