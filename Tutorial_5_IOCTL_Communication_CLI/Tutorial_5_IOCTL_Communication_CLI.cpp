#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <winioctl.h>  

#define SUM_IO_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)  
#define DIFF_IO_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)  
#define MSG_IO_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)  

static const std::string	gsc_szSymLink	= "\\\\.\\IOCTL_Test";
static HANDLE				gs_hDriver		= INVALID_HANDLE_VALUE;

typedef struct _KERNEL_IO_SUM_DATA
{
	INT iNumberFirst;
	INT iNumberSecond;
	INT iResult;
} SKernelIOSumData, *PKernelIOSumData;

typedef struct _KERNEL_IO_DIFF_DATA
{
	INT iNumberFirst;
	INT iNumberSecond;
	INT iResult;
} SKernelIODiffData, *PKernelIODiffData;

typedef struct _KERNEL_IO_MSG_DATA
{
	CHAR szMessage[255];
	BOOL bReceived;
} SKernelIOMsgData, *PKernelIOMsgData;

bool SumPacket()
{
	SKernelIOSumData pData;
	pData.iNumberFirst = 100;
	pData.iNumberSecond = 25;
	pData.iResult = 0;

	auto bRet = DeviceIoControl(gs_hDriver, SUM_IO_CODE, &pData, sizeof(pData), &pData, sizeof(pData), 0, NULL);
	printf("Sum IO completed! Result: %d Msg: %d\n", bRet, pData.iResult);

	return (bRet == TRUE);
}

bool DifferencePacket()
{
	SKernelIODiffData pData;
	pData.iNumberFirst = 50;
	pData.iNumberSecond = 33;
	pData.iResult = 0;

	auto bRet = DeviceIoControl(gs_hDriver, DIFF_IO_CODE, &pData, sizeof(pData), &pData, sizeof(pData), 0, NULL);
	printf("Diff IO completed! Result: %d Msg: %d\n", bRet, pData.iResult);

	return (bRet == TRUE);
}

bool MessagePacket()
{
	printf("Enter message: ");

	auto szMessage = std::string("");
	std::cin >> szMessage;

	SKernelIOMsgData pData;
	strcpy(pData.szMessage, szMessage.c_str());
	pData.bReceived = FALSE;

	auto bRet = DeviceIoControl(gs_hDriver, MSG_IO_CODE, &pData, sizeof(pData), &pData, sizeof(pData), 0, NULL);
	printf("Msg IO completed! Result: %d IsReceived: %d\n", bRet, pData.bReceived);

	return true;
}

int main()
{
	printf("Communication CLI started! Target device: %s\n", gsc_szSymLink.c_str());

	gs_hDriver = CreateFileA(gsc_szSymLink.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (!gs_hDriver || gs_hDriver == INVALID_HANDLE_VALUE)
	{
		printf("CreateFileA fail! Error: %u", GetLastError());
		return 1;
	}
	printf("Handle succesfully created: %p\n", gs_hDriver);

	printf("--------------------------------------\n");

	char pInput = '0';
	while (pInput != 'x')
	{
		printf("Please select:\n1 --> Sum packet\n2 --> Difference packet\n3 --> Message packet\nx --> Exit\n");
		std::cin >> pInput;

		switch (pInput)
		{
			case '1':
			{
				SumPacket();
			} break;

			case '2':
			{
				DifferencePacket();
			} break;
			
			case '3':
			{
				MessagePacket();
			} break;

			case 'x':
				return 0;

			default:
				continue;
		}
	}

    return 0;
}

