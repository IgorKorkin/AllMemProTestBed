
#include "windows.h"
#include "stdio.h"
#include "wchar.h"
#include <tchar.h>
#include <locale.h> // LC_ALL
#include "testbed.h"
#include "stdio.h"

#include <iostream>
#include <string>


using namespace std;


namespace check_windows_support {
	
	void set_minimal_os_info(OSVERSIONINFOEX & os_info) {
		/*
		Windows 7 Kernel Version 15063 MP (4 procs) Free x64
		Product: WinNt, suite: TerminalServer SingleUserTS
		Built by: 15063.0.amd64fre.rs2_release.170317-1834
		*/
		ZeroMemory(&os_info, sizeof(OSVERSIONINFOEX));
		os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		os_info.dwMajorVersion = 6;
		os_info.dwMinorVersion = 2;
		os_info.dwBuildNumber = 9200; // or 0x23f0
		os_info.wServicePackMajor = 0;
		os_info.wServicePackMinor = 0;
	}

	void print_windows_info(const char* title, const OSVERSIONINFOEX & os_version) {
		cout << title << endl;
		cout << " MajorVersion : " << std::dec << os_version.dwMajorVersion << std::hex << " (0x" << os_version.dwMajorVersion << ")" << endl;
		cout << " MinorVersion : " << std::dec << os_version.dwMinorVersion << std::hex << " (0x" << os_version.dwMinorVersion << ")" << endl;
		cout << " BuildNumber : " << std::dec << os_version.dwBuildNumber << std::hex << " (0x" << os_version.dwBuildNumber << ")" << endl;
		cout << " ServicePackMajor : " << std::dec << os_version.wServicePackMajor << std::hex << " (0x" << os_version.wServicePackMajor << ")" << endl;
		cout << " ServicePackMinor : " << std::dec << os_version.wServicePackMinor << std::hex << " (0x" << os_version.wServicePackMinor << ")" << endl;
	}

	bool is_ok() {
		OSVERSIONINFOEX os_info_needed = { 0 };
		set_minimal_os_info(os_info_needed);

		DWORDLONG dwlConditionMask = 0;
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, VER_EQUAL);

		auto b_res = (0 != VerifyVersionInfo(&os_info_needed,
			VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER | 
			VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, dwlConditionMask));

		if (b_res){
			cout << "This OS is supported!" << endl;
		}else if (ERROR_OLD_WIN_VERSION == GetLastError()){
			OSVERSIONINFOEX os_version = { 0 };
			RtlZeroMemory(&os_version, sizeof OSVERSIONINFOEX);
			os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
			if (GetVersionEx((LPOSVERSIONINFOW)&os_version)) {
				cout << "This OS is not supported" << endl;
				print_windows_info("Current OS details:", os_version);
				print_windows_info("The minimal OS requirements: Built by: 15063.0.amd64fre.rs2_release.170317-1834", os_info_needed);
			}
		}
		return b_res;
	}
} // namespace check_windows_support


void print_hello() {
	cout << endl;
	cout << "Press <key> and Enter  " << endl;
	cout << " 'b' -- run basic memory accesses " << endl;
	cout << " 's <BufferSize>' -- stack overflow with <BufferSize>" << endl;
	cout << " 'p <UniqueProcessId in dec>' -- stack overflow with payload (Privilege Escalation) " << endl;
	cout << " 'q' -- quit this app " << endl;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	argc; argv; envp; // to avoid warning C4100
	setlocale(LC_ALL, "");
	setvbuf(stdout, NULL, _IONBF, 0);
	if (check_windows_support::is_ok())
	{
		testbed :: TestBed my_testbed;
		if (my_testbed.is_ok()) {
			char command = 0;
			int bufsz = 0;
			int target_pid = 0;
			do {
				print_hello();
				command = 0; cin >> std::hex >> command;
				switch (command)
				{
				case 'b':
					my_testbed.run_basic_mem_access();
					break;
				case 's':
					bufsz = 0; cin >> std::dec >> bufsz;
					my_testbed.run_simple_stack_overflow(bufsz);
					break;
				case 'p':
					target_pid = 0; cin >> std::dec >> target_pid;
					my_testbed.run_stack_overflow_with_payload(target_pid);
					break;
				default: {};
						 break;
				}
			} while ('q' != command);
		}
	}
	cin.ignore();
	cout << "Press enter to exit." << endl;
	cin.ignore(); // std::system("PAUSE");
}