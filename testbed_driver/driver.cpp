// Copyright (c) 2015-2016, tandasat. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

/// @file
/// Implements an entry point of the driver.

#ifndef POOL_NX_OPTIN
#define POOL_NX_OPTIN 1
#endif
#include "driver.h"


extern "C" {
////////////////////////////////////////////////////////////////////////////////
//
// macro utilities
//

////////////////////////////////////////////////////////////////////////////////
//
// constants and macros
//

////////////////////////////////////////////////////////////////////////////////
//
// types
//

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

DRIVER_INITIALIZE DriverEntry;

static DRIVER_UNLOAD DriverpDriverUnload;

_IRQL_requires_max_(PASSIVE_LEVEL) bool DriverpIsSuppoetedOS();

#if defined(ALLOC_PRAGMA)
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverpDriverUnload)
#pragma alloc_text(INIT, DriverpIsSuppoetedOS)
#endif

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

static basic_mem_access::BasicMemoryAccess g_basic_access;
////////////////////////////////////////////////////////////////////////////////
//
// implementations
//



void remove_symbol_link(PWCHAR linkName){
	UNICODE_STRING device_link;
	RtlInitUnicodeString(&device_link, linkName);
	IoDeleteSymbolicLink(&device_link);
}

void remove_control_device(PDRIVER_OBJECT driver_object) {
	PDEVICE_OBJECT device_object = driver_object->DeviceObject;
	while (device_object){
		IoDeleteDevice(device_object);
		device_object = device_object->NextDevice;
	}
}

// Unload handler
_Use_decl_annotations_ static void DriverpDriverUnload(
    PDRIVER_OBJECT driver_object) {
  UNREFERENCED_PARAMETER(driver_object);
  PAGED_CODE();
  //TESTBED_COMMON_DBG_BREAK();

  remove_symbol_link(TESTBED_LINKNAME_APP);
  remove_control_device(driver_object);

  DbgPrint("The driver [%ws] has been unloaded! \r\n", TESTBED_SYS_FILE);
}

// Create-Close handler
_Use_decl_annotations_ NTSTATUS DriverpCreateClose(IN PDEVICE_OBJECT pDeviceObject, IN PIRP  Irp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	const auto stack = IoGetCurrentIrpStackLocation(Irp);
	switch (stack->MajorFunction) {
		case IRP_MJ_CREATE: break;
		case IRP_MJ_CLOSE: break;
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}


// Read Write handler
_Use_decl_annotations_ static NTSTATUS DriverpReadWrite(IN PDEVICE_OBJECT pDeviceObject, IN PIRP  Irp){
	PAGED_CODE();

	PVOID buf = NULL;
	auto buf_size = 0;
	// Read size of input buffer 
	const auto stack = IoGetCurrentIrpStackLocation(Irp);
	switch (stack->MajorFunction){
		case IRP_MJ_READ: buf_size = stack->Parameters.Read.Length; break;
		case IRP_MJ_WRITE: buf_size = stack->Parameters.Write.Length; break;
	}
	// Get the address of input buffer
	if (buf_size){
		if (pDeviceObject->Flags & DO_BUFFERED_IO) {
			buf = Irp->AssociatedIrp.SystemBuffer;
		}
		else if (pDeviceObject->Flags & DO_DIRECT_IO) {
			buf = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
		}
		else {
			buf = Irp->UserBuffer;
		}
	}

	// Do nothing and complete request
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

_Use_decl_annotations_ static void read_param(IN PIRP pIrp, 
	OUT PVOID &inBuf, OUT ULONG &inBufSize, 
	OUT PVOID &outBuf, OUT ULONG &outBufSize){
	const auto stack = IoGetCurrentIrpStackLocation(pIrp);
	inBufSize = stack->Parameters.DeviceIoControl.InputBufferLength;
	outBufSize = stack->Parameters.DeviceIoControl.OutputBufferLength;
	const auto method = stack->Parameters.DeviceIoControl.IoControlCode & 0x03L;
	switch (method)
	{
	case METHOD_BUFFERED:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;
		outBuf = pIrp->AssociatedIrp.SystemBuffer;
		break;
	case METHOD_IN_DIRECT:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;
		outBuf = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
		break;
	case METHOD_OUT_DIRECT:
		inBuf = pIrp->AssociatedIrp.SystemBuffer;
		outBuf = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
		break;
	case METHOD_NEITHER:
		inBuf = stack->Parameters.DeviceIoControl.Type3InputBuffer;
		outBuf = pIrp->UserBuffer;
		break;
	}
}

// IOCTL dispatch handler
_Use_decl_annotations_ static NTSTATUS DriverpDeviceControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PAGED_CODE();

	const auto stack = IoGetCurrentIrpStackLocation(pIrp);
	PVOID in_buf = NULL, out_buf = NULL;
	ULONG in_buf_sz = 0, out_buf_sz = 0;
	auto status = STATUS_INVALID_PARAMETER;
	ULONG_PTR info = 0;
	read_param(pIrp, in_buf, in_buf_sz, out_buf, out_buf_sz);
	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
		case TESTBED_BASIC_MEMORY_ACCESS:
			status = g_basic_access.basic_memory_accesses();
			info = in_buf_sz;
			break;
		case TESTBED_SIMPLE_STACK_OVERFLOW:
			status = vulnerable_code::stack_overflow_stub(in_buf, in_buf_sz);
			info = in_buf_sz;
			break;
		case TESTBED_STACK_OVERFLOW_FROM_HEVD:
			/* Wrong function call, because we need a second function-stub, which will be called by TriggerStackOverflow() */
			status = vulnerable_code::TriggerStackOverflow(in_buf, in_buf_sz);
			info = in_buf_sz;
			break;
		case TESTBED_UAF_ALLOCATE_OBJECT:
			status = vulnerable_code::uaf_allocate_object_stub();
			info = in_buf_sz;
			break;
		case TESTBED_UAF_FREE_OBJECT:
			status = vulnerable_code::uaf_free_object_stub();
			info = in_buf_sz;
			break;
		case TESTBED_UAF_USE_OBJECT:
			status = vulnerable_code::uaf_use_object_stub();
			info = in_buf_sz;
			break;
		case TESTBED_UAF_ALLOCATE_FAKE:
			status = vulnerable_code::uaf_allocate_fake_stub(in_buf);
			info = in_buf_sz;
			break;
		default: {}
	}

	pIrp->IoStatus.Information = info;
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}



// Test if the system is one of supported OS versions
_Use_decl_annotations_ bool DriverpIsSuppoetedOS() {
  PAGED_CODE();

  RTL_OSVERSIONINFOW os_version = {};
  auto status = RtlGetVersion(&os_version);
  if (!NT_SUCCESS(status)) {
    return false;
  }

  if (os_version.dwBuildNumber != 15063){
	  return false;
  }

  // 4-gigabyte tuning (4GT) should not be enabled
  if (!IsX64() &&
      reinterpret_cast<ULONG_PTR>(MmSystemRangeStart) != 0x80000000) {
    return false;
  }
  return true;
}

_Use_decl_annotations_ NTSTATUS create_device(IN PDRIVER_OBJECT pDrv, ULONG uFlags, PWCHAR devName, PWCHAR linkName)
{
	UNICODE_STRING dev_name = { 0 }, link_name = {0};
	RtlInitUnicodeString(&dev_name, devName);
	RtlInitUnicodeString(&link_name, linkName);

	PDEVICE_OBJECT pDev;
	auto status = IoCreateDevice(pDrv, 0 /* or sizeof(DEVICE_EXTENSION)*/, &dev_name, 65500, 0, 0, &pDev);

	if (NT_SUCCESS(status)) {
		pDev->Flags |= uFlags;
		IoDeleteSymbolicLink(&link_name);
		status = IoCreateSymbolicLink(&link_name, &dev_name);
	}
	else   {   IoDeleteDevice(pDev);   }

	return status;
}

#include "intrin.h"
static const unsigned long SMEP_MASK = 0x100000;

void print_smep_status() {
	bool b_active = false;
	KAFFINITY active_processors = KeQueryActiveProcessors();
	for (KAFFINITY current_affinity = 1; active_processors; current_affinity <<= 1) {
		if (active_processors & current_affinity) {
			active_processors &= ~current_affinity;
			KeSetSystemAffinityThread(current_affinity);
			b_active = (0 != (__readcr4() & SMEP_MASK));
			DbgPrint("%s on CPU %d \r\n",
				b_active ? "SMEP is active" : "SMEP has been disabled",
				KeGetCurrentProcessorNumber() );
		}
	}
}

// A driver entry point
_Use_decl_annotations_ NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object,
	PUNICODE_STRING registry_path) {
	UNREFERENCED_PARAMETER(registry_path);
	PAGED_CODE();
	
	DbgPrint("********************************* \r\n");
	DbgPrint("The driver [%ws] has been loaded! \r\n\r\n\r\n", TESTBED_SYS_FILE);

	// Test if the system is supported
	if (!DriverpIsSuppoetedOS()) {
		return STATUS_CANCELLED;
	}

	print_smep_status();

	driver_object->DriverUnload = DriverpDriverUnload;
	driver_object->MajorFunction[IRP_MJ_CREATE] =
	driver_object->MajorFunction[IRP_MJ_CLOSE] = DriverpCreateClose;
	driver_object->MajorFunction[IRP_MJ_READ] =
	driver_object->MajorFunction[IRP_MJ_WRITE] = DriverpReadWrite;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverpDeviceControl;

	auto nt_status = create_device(driver_object, NULL, TESTBED_DEVICENAME_DRV, TESTBED_LINKNAME_DRV);

	if (NT_SUCCESS(nt_status)){
		g_basic_access.init();
	}
	return nt_status;
}

}  // extern "C"
