#pragma once
#pragma warning (disable : 4100)

#include <ntifs.h>
#include "datatype.h"

DWORD LockAcquired;
DWORD nCPUsLocked;

KIRQL RaiseIRQL();

void lockRoutine(IN PKDPC dpc, IN PVOID context, IN PVOID arg1, IN PVOID arg2);

PKDPC AcquireLock();

NTSTATUS ReleaseLock(PVOID dpcPtr);

void LowerIRQL(KIRQL prev);
