#pragma once

#include <Windows.h>
#include <stdio.h>

#define IO_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

class Communicator
{
public:
	HANDLE hDriver;

	Communicator(LPCSTR RegistryPath);
	bool hideTask(DWORD pid);
};

