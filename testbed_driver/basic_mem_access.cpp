

#include "basic_mem_access.h"

extern "C" namespace basic_mem_access 
{
	TESTBED_STR _GlobalStruct = {0};

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
		DbgPrint("[Function], addr 0x%.16I64X \r\n", &BasicMemoryAccess::basic_memory_accesses);

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
}