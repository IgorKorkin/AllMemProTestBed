

#include "basic_mem_access.h"


/*
;++
;
; VOID
; RtlpGetStackLimits (
;    OUT PULONG LowLimit,
;    OUT PULONG HighLimit
;    )
;
; Routine Description:
;
;    This function returns the current stack limits based on the current
;    processor mode.
;
; Arguments:
;
;    LowLimit (esp+4) - Supplies a pointer to a variable that is to receive
;       the low limit of the stack.
;
;    HighLimit (esp+8) - Supplies a pointer to a variable that is to receive
;       the high limit of the stack.
;
; Return Value:
;
;    None.
;
;--

cPublicProc _RtlpGetStackLimits ,2
;cPublicFpo 2,0


mov     eax,fs:PcPrcbData+PbCurrentThread ; get current thread address
mov     eax,[eax]+ThStackLimit  ; get thread stack limit

mov     ecx,[esp+4]
mov     [ecx],eax               ; Save low limit  

 /// * +0x030 StackLimit       : 0xa8da1c80`ffffad81 Void
 /// * or ffffad81 a8da1c80

mov     eax,fs:PcPrcbData+PbCurrentThread ; get current thread address
mov     eax,[eax].ThInitialStack
sub     eax, NPX_FRAME_LENGTH
mov     ecx,[esp+8]
mov     [ecx],eax               ; Save high limit

 /// * 0x028 InitialStack     : 0xa8d9fc80`ffffad81
 /// * or ffffad81 a8d9fc80

stdRET    _RtlpGetStackLimits

stdENDP _RtlpGetStackLimits





*/


extern "C" namespace basic_mem_access 
{
	TESTBED_STR _GlobalStruct = { 0 };

	CONFIG_THREAD configThread = { 0 };

	// Initialize global buffers
	_Use_decl_annotations_ void BasicMemoryAccess::init() {
		_pGlobalStrPaged =
			reinterpret_cast<TESTBED_STR *>(ExAllocatePool(PagedPool, sizeof(TESTBED_STR)));
		if (_pGlobalStrPaged) {
			RtlSecureZeroMemory(_pGlobalStrPaged, sizeof(TESTBED_STR));
		}
		_pGlobalStrNonPaged =
			reinterpret_cast<TESTBED_STR *>(ExAllocatePool(NonPagedPool, sizeof(TESTBED_STR)));
		if (_pGlobalStrNonPaged) {
			RtlSecureZeroMemory(_pGlobalStrNonPaged, sizeof(TESTBED_STR));
		}
	}
		
	// Print buffers
	_Use_decl_annotations_ void BasicMemoryAccess::print_struct(const char* title, const TESTBED_STR * str) {
		DbgPrint("[%10s] \t at addr 0x%.16I64X, content [%X] [%s] \r\n",
			title, 
			str, 
			str->one_byte, 
			str->buf);
	}

	_Use_decl_annotations_ NTSTATUS BasicMemoryAccess::basic_memory_accesses() {

		ULONG_PTR LowLimit = { 0 };
		ULONG_PTR HighLimit = { 0 };
		IoGetStackLimits(&LowLimit, &HighLimit);
		DbgPrint("Stack limits [%s] %I64X-%I64X \r\n",
			__FUNCTION__, LowLimit, HighLimit);

		__int64 rsp_value = AsmReadRSP();
		DbgPrint("RSP value %I64X \r\n",
			rsp_value);

		DbgPrint("[Function], addr %.16I64X \r\n", &BasicMemoryAccess::basic_memory_accesses);

		char * patch_byte = (char*)PsInitialSystemProcess;
		char first_byte = *patch_byte; // = 'I';
		char twentieth_byte = *(patch_byte + 20); //*(patch_byte + 20) = 'G';
			DbgPrint("   EPROCESS %.16I64X [0] = %02X, [20] = %02X \r\n", 
				PsInitialSystemProcess, 
				first_byte, 
				twentieth_byte);

		patch_byte = (char*)0xFFFF800000000000;
		__try {
			first_byte = *patch_byte;
		}
		__except (EXCEPTION_EXECUTE_FAULT) {
			first_byte = 0;
		}

		DbgPrint("   %.16I64X [0] = %02X \r\n",
			patch_byte,
			first_byte);

		// 1 Access to local variable on stack memory
		TESTBED_STR local_struct = { 0 };
		local_struct.one_byte = 0x11;
		memcpy(local_struct.buf, "LocalVar", sizeof(local_struct.buf));
		print_struct("Local variable", &local_struct);

		// 2 Access to global variable
		RtlSecureZeroMemory(&_GlobalStruct, sizeof(TESTBED_STR));
		_GlobalStruct.one_byte = 0x22;
		memcpy(_GlobalStruct.buf, "GlobalVar", sizeof(local_struct.buf));
		print_struct("Global variable", &_GlobalStruct);

		static TESTBED_STR stat_local_struct = { 0 };
		// 3 Access to static global variable
		RtlSecureZeroMemory(&stat_local_struct, sizeof(TESTBED_STR));
		stat_local_struct.one_byte = 0x33;
		memcpy(stat_local_struct.buf, "LocalStatic", sizeof(local_struct.buf));
		print_struct("Static local variable", &stat_local_struct);

		// 5 Allocate local variable on the heap and write to it 
		const auto local_paged_buf =
			reinterpret_cast<TESTBED_STR *>(ExAllocatePool(PagedPool, sizeof(TESTBED_STR)));
		if (local_paged_buf) {
			RtlSecureZeroMemory(local_paged_buf, sizeof(TESTBED_STR));
			local_paged_buf->one_byte = 0x55;
			memcpy(local_paged_buf->buf, "AllocLocalPaged", sizeof(local_paged_buf->buf));
			print_struct("Allocated Local Paged", local_paged_buf);
			ExFreePool(local_paged_buf);
		}

		// 5 Allocate local variable on the heap and write to it 
		const auto local_nonpaged_buf =
			reinterpret_cast<TESTBED_STR *>(ExAllocatePool(NonPagedPool, sizeof(TESTBED_STR)));
		if (local_nonpaged_buf) {
			RtlSecureZeroMemory(local_nonpaged_buf, sizeof(TESTBED_STR));
			local_nonpaged_buf->one_byte = 0x55;
			memcpy(local_nonpaged_buf->buf, "AllocLocalNonPaged", sizeof(local_nonpaged_buf->buf));
			print_struct("Allocated Local NonPaged", local_nonpaged_buf);
			ExFreePool(local_nonpaged_buf);
		}

		// 6 Access to allocated Paged-global variable
		if (_pGlobalStrPaged) {
			RtlSecureZeroMemory(_pGlobalStrPaged, sizeof(TESTBED_STR));
			_pGlobalStrPaged->one_byte = 0x66;
			memcpy(_pGlobalStrPaged->buf, "GlobPagedBuf", sizeof(local_struct.buf));
			print_struct("Allocated Global Paged", _pGlobalStrPaged);
		}

		// 7 Access to allocated NonPaged-global variable
		if (_pGlobalStrNonPaged) {
			RtlSecureZeroMemory(_pGlobalStrNonPaged, sizeof(TESTBED_STR));
			_pGlobalStrNonPaged->one_byte = 0x77;
			memcpy(_pGlobalStrNonPaged->buf, "GlobNonPagedBuf", sizeof(local_struct.buf));
			print_struct("Allocated Global NonPaged", _pGlobalStrNonPaged);
		}
		return STATUS_SUCCESS;
	}

	//////////////////////////////////////////////////////////////////////////

	void set_print_proc_info(void *buf, ULONG sz) {
		RtlSecureZeroMemory(buf, sz);
		NTSTATUS nt_status = ZwQuerySystemInformation(SystemBasicInformation, buf, sz, &sz);
		PSYSTEM_BASIC_INFORMATION psys_info = (PSYSTEM_BASIC_INFORMATION)buf;
		if (NT_SUCCESS(nt_status)) {

			DbgPrint("Phys range %I64X-%I64X pages %I64X \r\n", 
				psys_info->LowestPhysicalPage,
				psys_info->HighestPhysicalPage, 
				psys_info->NumberOfPhysicalPages);

// 			while (psys_info->NextEntryOffset){
// 				psys_info = (PSYSTEM_PROCESS_INFORMATION)((char *)psys_info + psys_info->NextEntryOffset);
// 				DbgPrint(" (%wZ : %X) ", &psys_info->ImageName, psys_info->UniqueProcessId);
// 			}
// 			DbgPrint("\r\n");
		}
	}

	VOID memory_access_loop(_In_ PVOID StartContext) {
		configThread.flagLoopIsActive = true;
		REACTOR_CONFIG* p_data = (REACTOR_CONFIG*)StartContext;
		LARGE_INTEGER timeout = { 0 };
		timeout.QuadPart = (LONGLONG)(-1000 * 1000 * 10 * 2);  //  2s
		ULONG64 data = 0;
		while (configThread.flagLoopIsActive){

			__try {
				KeWaitForMutexObject(&configThread.mutex, Executive, KernelMode, FALSE, NULL);
				data = p_data->tempReactor;
				set_print_proc_info(p_data->buf_proc_info, p_data->buf_proc_info_sz);
				KeReleaseMutex(&configThread.mutex, FALSE);
			}
			__except (EXCEPTION_EXECUTE_FAULT) {
				data = 0;
			}
			DbgPrint("Temp is   [ %d %X ]   from %I64X \r\n", data, data, &p_data->tempReactor );
			DbgPrint("Proc info is from %I64X - %I64X \r\n", p_data->buf_proc_info, (char*)p_data->buf_proc_info + p_data->buf_proc_info_sz);
			KeDelayExecutionThread(KernelMode, FALSE, &timeout);
		}
	}

	NTSTATUS BasicMemoryAccess::set_memory_thread(void* buf, void *outbuf) {
		REACTOR_CONFIG* p_data = (REACTOR_CONFIG*)configThread.pconfig_data;

		KeWaitForMutexObject(&configThread.mutex, Executive, KernelMode, FALSE, NULL);
		p_data->tempReactor = *(ULONG64*)buf;
		KeReleaseMutex(&configThread.mutex, FALSE);

		*((ULONG64*)outbuf) = ((ULONG64)&p_data->tempReactor);

		return STATUS_SUCCESS;
	}

	NTSTATUS BasicMemoryAccess::start_set_thread(void* inbuf, void *outbuf){
		if (configThread.flagLoopIsActive){
			return 
				set_memory_thread(inbuf, outbuf);
		}
		else {
			KeInitializeMutex(&configThread.mutex, 0);
			REACTOR_CONFIG *pinit_data = (REACTOR_CONFIG*)ExAllocatePool(
				NonPagedPool, sizeof REACTOR_CONFIG);
			RtlSecureZeroMemory(pinit_data, sizeof REACTOR_CONFIG);
			pinit_data->tempReactor = *(ULONG64*)inbuf;
			*((ULONG64*)outbuf) = (ULONG64)&pinit_data->tempReactor;

			pinit_data->buf_proc_info_sz = 0;
			ZwQuerySystemInformation(SystemBasicInformation, NULL, 0, &pinit_data->buf_proc_info_sz);
			pinit_data->buf_proc_info = ExAllocatePool(NonPagedPool, pinit_data->buf_proc_info_sz);
			if (pinit_data->buf_proc_info){
				set_print_proc_info(pinit_data->buf_proc_info, pinit_data->buf_proc_info_sz);
			}
			

			configThread.pconfig_data = pinit_data;
			NTSTATUS nt_status =
				PsCreateSystemThread(
					&configThread.handleMemoryLoop,
					THREAD_ALL_ACCESS, NULL, NULL, NULL,
					memory_access_loop, pinit_data);

			if (!NT_SUCCESS(nt_status)) {
				DbgPrint(" PsCreateSystemThread error %08X \r\n", nt_status);
			}
			return nt_status;
		}
	}

	NTSTATUS BasicMemoryAccess::stop_thread() {
		configThread.flagLoopIsActive = false;
		NTSTATUS nt_status = ObReferenceObjectByHandle(configThread.handleMemoryLoop, 
			THREAD_ALL_ACCESS, NULL, KernelMode, (PVOID*)&configThread.pthread, NULL);
		if (NT_SUCCESS(nt_status)){

			if (STATUS_SUCCESS == (nt_status = KeWaitForSingleObject(configThread.pthread,
				Executive, KernelMode, FALSE, NULL))){
				ObDereferenceObject(configThread.pthread);
				if (configThread.pconfig_data){
					ExFreePool(configThread.pconfig_data);
				}
			}
		}
		return nt_status;
	}
}