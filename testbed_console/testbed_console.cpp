#include "testbed_console.h"

namespace resource_functions
{
	typedef struct _RESOURCE
	{
		LPVOID  ResContent;
		DWORD   ResSize;
		HRSRC   hResFile;
		HGLOBAL hResData;
	}RESOURCE, *PRESOURCE;

	bool extract_resource(RESOURCE & resourse, LPCTSTR lpName, LPCTSTR lpType)
	{
		auto b_res = false;
		resourse.hResFile = FindResource(NULL, lpName, lpType);
		if (resourse.hResFile)
		{
			resourse.ResSize = SizeofResource(NULL, resourse.hResFile);
			if (resourse.ResSize)
			{
				resourse.hResData = LoadResource(NULL, resourse.hResFile);
				if (resourse.hResData)
				{
					resourse.ResContent = LockResource(resourse.hResData);
					if (resourse.ResContent) { b_res = true; }
				}
			}
		}
		return b_res;
	}

	bool get_temp_filename(LPCTSTR lpPrefixString, LPTSTR lpTempFileName)
	{
		auto b_res = false;
		wchar_t tmp_path[MAX_PATH] = { 0 };
		if (GetTempPath(MAX_PATH, tmp_path) &&
			GetTempFileName(tmp_path, lpPrefixString, 0, lpTempFileName))
		{
			b_res = true;
		}
		return b_res;
	}

}

namespace testbed_console
{
	bool extract_driver_file(TCHAR * binFile) {
		auto b_res = false;
		resource_functions::RESOURCE resourse = { 0 }; RtlSecureZeroMemory(&resourse, sizeof(resource_functions::RESOURCE));

		if (resource_functions::extract_resource(resourse, MAKEINTRESOURCE(TESTBED_BIN_SYSFILE), TEXT("bin")))
		{
			if (resource_functions::get_temp_filename(TEXT("TB"), binFile))
			{
				HANDLE h_file = NULL;
				if (PathFileExists(binFile) &&
					((HANDLE)INVALID_HANDLE_VALUE != (h_file = CreateFile(binFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL))))
				{
					DWORD number_of_bytes_written = 0;
					if (WriteFile(h_file, resourse.ResContent, resourse.ResSize, &number_of_bytes_written, NULL) &&
						(number_of_bytes_written == resourse.ResSize)){
						b_res = true;
					}
				}
				else{
					print::print_last_err(L"err file %s ", binFile); 
				}
				if (h_file) { CloseHandle(h_file); }
			}
		}
		return b_res;
	}

	bool load_driver_from_file(TCHAR * binFile) {
		auto b_res = false;
		send_ctl_codes::m_smCsmgr.set_names(TESTBED_SERVNAME_APP, binFile);
		send_ctl_codes::m_smCsmgr.stop_driver();
		send_ctl_codes::m_smCsmgr.remove_driver();
		if (send_ctl_codes::m_smCsmgr.add_driver()) {
			if (send_ctl_codes::m_smCsmgr.start_driver()) {
				if (send_ctl_codes::m_smCsmgr.open_device(TESTBED_LINKNAME_APP) != (HANDLE)INVALID_HANDLE_VALUE)	{
					b_res = true;
					print::print_mes(L"The [%s] driver is active!", TESTBED_SERVNAME_APP);
					//	*for debugging process:
					// 	m_smCsmgr.close_device(m_hNeither);
					// 	m_smCsmgr.stop_driver();
					// 	m_smCsmgr.remove_driver();
				}
				else {
						print::print_last_err(L"err open device %s ", TESTBED_LINKNAME_APP);
					}
			}
			else {
				print::print_last_err(L"err start driver %s %s", TESTBED_SERVNAME_APP, binFile);
				send_ctl_codes::m_smCsmgr.stop_driver();
				send_ctl_codes::m_smCsmgr.remove_driver();
			}
		}
		else {
			print::print_last_err(L"err add driver %s %s", TESTBED_SERVNAME_APP, binFile);
			send_ctl_codes::m_smCsmgr.stop_driver();
			send_ctl_codes::m_smCsmgr.remove_driver();
		}
		send_ctl_codes::m_smCsmgr.delete_binfile();
		return b_res;
	}

	bool activate_testbed()
	{
		TCHAR bin_file[MAX_PATH] = { 0 };
		return extract_driver_file(bin_file) && load_driver_from_file(bin_file);
	}

	bool set_current_dir(wchar_t * logFolder)
	{
		auto b_res = false;
		DWORD len = GetModuleFileName(NULL, logFolder, MAX_PATH);
		if (len)
		{
			while (len && logFolder[len] != TEXT('\\'))
			{
				len--;
			}

			if (len)
			{
				logFolder[len + 1] = 0;
				if (!wcsncat_s(logFolder, MAX_PATH, TEXT("logs\\"), _TRUNCATE))
				{
					b_res = true;
				}
			}
		}
		return b_res;
	}

	bool create_log_folder(wchar_t * logFolder)
	{
		auto b_res = false;
		auto len = GetModuleFileName(NULL, logFolder, MAX_PATH);
		if (len)
		{
			while (len && logFolder[len] != TEXT('\\'))
			{
				len--;
			}

			if (len)
			{
				logFolder[len + 1] = 0;
				if ((!wcsncat_s(logFolder, MAX_PATH, TEXT("logs\\"), _TRUNCATE)) &&
					(CreateDirectory(logFolder, NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
				{
					UNICODE_STRING object_name;
					if (RtlDosPathNameToNtPathName_U(logFolder, &object_name, NULL, NULL))
					{
						if (_stprintf_s(logFolder, MAX_PATH, object_name.Buffer) >= 0)
						{
							b_res = true;
						}
					}
				}
			}
		}
		return b_res;
	}
}