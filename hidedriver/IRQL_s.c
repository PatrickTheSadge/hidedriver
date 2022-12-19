#include "IRQL_s.h"
#include "messages.h"
#include "datatype.h"
#include "dbgmsg.h"

KIRQL RaiseIRQL()
{
	KIRQL curr;
	KIRQL prev;
	curr = KeGetCurrentIrql();
	prev = curr;
	if (curr < DISPATCH_LEVEL)
		KeRaiseIrql(DISPATCH_LEVEL, &prev);
	return prev;
}

void lockRoutine(IN PKDPC dpc, IN PVOID context, IN PVOID arg1, IN PVOID arg2)
{
	DBG_PRINT2("[lockRoutine]: begin-CPU[%u]", KeGetCurrentProcessorNumber());
	InterlockedIncrement(&nCPUsLocked);

	while (InterlockedCompareExchange(&LockAcquired, 1, 1) == 0)
	{
		__asm
		{
			nop;
		}
	}

	InterlockedDecrement(&nCPUsLocked);
	DBG_PRINT2("[lockRoutine]: end-CPU[%u]", KeGetCurrentProcessorNumber());
	return;
}

PKDPC AcquireLock()
{
	PKDPC dpcArray;
	DWORD cpuID;
	DWORD i;
	DWORD nOtherCPUs;
	if (KeGetCurrentIrql() != DISPATCH_LEVEL)
		return NULL;
	DBG_TRACE("AcquireLock", "Executing at IRQL==DISPATCH_LEVEL");

	//init globals to zero
	InterlockedAnd(&LockAcquired, 0);
	InterlockedAnd(&nCPUsLocked, 0);

	DBG_PRINT2("[AcquireLock]: nCPUs=%u\n", KeNumberProcessors);
	dpcArray = (PKDPC)ExAllocatePool(NonPagedPool, KeNumberProcessors*sizeof(KDPC));
	if (dpcArray == NULL)
		return NULL;

	cpuID = KeGetCurrentProcessorNumber();
	DBG_PRINT2("[AcquireLock]: cpuID=%u\n", cpuID);

	for (i = 0; i < KeNumberProcessors; i++)
	{
		PKDPC dpcPtr = &(dpcArray[i]);
		if (i != cpuID)
		{
			KeInitializeDpc(dpcPtr, lockRoutine, NULL);
			KeSetTargetProcessorDpc(dpcPtr, (CCHAR)i);
			KeInsertQueueDpc(dpcPtr, NULL, NULL);
		}
	}

	nOtherCPUs = KeNumberProcessors - 1;
	InterlockedCompareExchange(&nCPUsLocked, nOtherCPUs, nOtherCPUs);
	while (nCPUsLocked != nOtherCPUs)
	{
		__asm
		{
			nop;
		}
		InterlockedCompareExchange(&nCPUsLocked, nOtherCPUs, nOtherCPUs);
	}
	DBG_TRACE("AcquireLock", "All CPUs have been elevated");
	return dpcArray;
}

NTSTATUS ReleaseLock(PVOID dpcPtr)
{
	InterlockedIncrement(&LockAcquired);

	InterlockedCompareExchange(&nCPUsLocked, 0, 0);
	while (nCPUsLocked != 0)
	{
		__asm
		{
			nop;
		}
		InterlockedCompareExchange(&nCPUsLocked, 0, 0);
	}
	if (dpcPtr != NULL)
		ExFreePool(dpcPtr);

	DBG_TRACE("ReleaseLock", "All CPUs have been released");
	return STATUS_SUCCESS;
}

void LowerIRQL(KIRQL prev)
{
	KeLowerIrql(prev);
	return;
}

/*void Unload(IN PDRIVER_OBJECT pDriverObject)
{
	DBG_TRACE("Unload", "Received signal to unload the driver");
	return;
}*/

/*NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING regPath)
{
	NTSTATUS ntStatus;
	KIRQL irql;
	PKDPC dpcPtr;

	DBG_TRACE("Driver Entry", "Establishing other DriverObject function pointers");
	pDriverObject->DriverUnload = Unload;

	DBG_TRACE("Driver Entry", "Raising IRQL");
	irql = RaiseIRQL();

	DBG_TRACE("Driver Entry", "Acquiring Lock");
	dpcPtr = AcquireLock();

	//tutaj shared resources access mozna bo mamy locka na calutki processor

	DBG_TRACE("Driver Entry", "Releasing Lock");
	ReleaseLock(dpcPtr);

	DBG_TRACE("Driver Entry", "Lowering IRQL");
	LowerIRQL(irql);
	return STATUS_SUCCESS;
}*/