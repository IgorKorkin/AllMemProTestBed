
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

const char name_basic[] = "basic";
const int command_basic = 100;

const char name_test_stack[] = "test_stack";
const int command_test_stack = 110;

const char name_stack[] = "stack";
const int command_stack = 111;

const char name_test_uaf[] = "test_uaf";
const int command_test_uaf = 120;

const char name_uaf[] = "uaf";
const int command_uaf = 121;

const char name_test_pool[] = "test_pool";
const int command_test_pool = 131;

const char name_pool[] = "pool";
const int command_pool = 132;

const char name_starttemp[] = "starttemp";
const int command_starttemp = 133;

const char name_stoptemp[] = "stoptemp";
const int command_stoptemp = 135;

const char name_exit[] = "exit";
const int command_exit = 0xFFF1;

const char name_quit[] = "q";
const int command_quit = 0xFFF2;

void print_hello() {
	std::cout << endl;
	std::cout << "<<TestBed console for Windows Kernel Exploitation >>" << endl;
	std::cout << " '" << name_basic << "' -- run basic memory accesses " << endl;
	std::cout << " '" << name_test_stack <<" <BufferSize>' -- test stack overflow with <BufferSize>" << endl;
	std::cout << " '" << name_stack << " <UniqueProcessId in dec>' -- stack overflow with payload for Privilege Escalation " << endl;
	std::cout << " '" << name_test_uaf <<"' -- run simple use after free, which cause a BSOD " << endl;
	std::cout << " '" << name_uaf << " <UniqueProcessId in dec>' -- run use after free with fake object and payload " << endl;
	std::cout << " '" << name_test_pool << "' -- test pool functions" << endl;
	std::cout << " '" << name_pool << " <BufferSize>' -- test pool overflow with <BufferSize>" << endl;
	
	std::cout << " '" << name_starttemp << " <temp>' -- start and set temp loop " << endl;
	std::cout << " '" << name_stoptemp << "' -- stop loop" << endl;
	
	std::cout << " '" << name_exit << "' -- exit this app " << endl;
}

int parse(string mystring) {
	int i_res = 0x12345678;

	if (std::string::npos != mystring.find(name_basic)){
		i_res = command_basic;
	}
	else if (std::string::npos != mystring.find(name_test_stack)) {
		i_res = command_test_stack;
	}
	else if (std::string::npos != mystring.find(name_stack)) {
		i_res = command_stack;
	}
	else if (std::string::npos != mystring.find(name_test_uaf)) {
		i_res = command_test_uaf;
	}
	else if (std::string::npos != mystring.find(name_uaf)) {
		i_res = command_uaf;
	}
	else if (std::string::npos != mystring.find(name_test_pool)) {
		i_res = command_test_pool;
	}
	else if (std::string::npos != mystring.find(name_pool)) {
		i_res = command_pool;
	}

	else if (std::string::npos != mystring.find(name_starttemp)) {
		i_res = command_starttemp;
	}
	else if (std::string::npos != mystring.find(name_stoptemp)) {
		i_res = command_stoptemp;
	}
	
	else if (std::string::npos != mystring.find(name_quit)) {
		i_res = command_quit;
	}

	else if (std::string::npos != mystring.find(name_exit)) {
		i_res = command_exit;
	}
	return i_res;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	argc; argv; envp; // to avoid warning C4100
	setlocale(LC_ALL, "");
	setvbuf(stdout, NULL, _IONBF, 0);

//	if (check_windows_support::is_ok()) 
	{
		testbed_for_exploitation :: TestBed my_testbed; // activate testbed
		if (my_testbed.is_ok())
		{
			string string_command = { 0 };
			int code_command = 0;
			ULONG64 temp = 0;
			int bufsz = 0;
			int target_pid = 0;
			do {
				print_hello();
				string_command = { 0 };
				std::cin.clear();
				std::cin >> string_command; //std::getline(std::cin >> std::ws, string_command);
				code_command = parse(string_command);
				switch (code_command)
				{
				case command_basic:
					my_testbed.run_basic_mem_access();
					break;
				case command_test_stack:
					bufsz = 0; std::cin >> std::dec >> bufsz;
					my_testbed.run_simple_stack_overflow(bufsz);
					break;
				case command_stack:
					target_pid = 0; std::cin >> std::dec >> target_pid;
					my_testbed.run_stack_overflow_with_payload(target_pid);
					break;
				case command_test_uaf:
					my_testbed.run_use_after_free();
					break;
				case command_uaf:
					target_pid = 0; std::cin >> std::dec >> target_pid;
					my_testbed.run_use_after_free_with_payload(target_pid);
					break;
				case command_test_pool:
					my_testbed.test_pool_allocations();
					break;
				case command_pool:
					bufsz = 0; std::cin >> std::dec >> bufsz;
					my_testbed.run_pool_overflow(bufsz);
					break;
				case command_starttemp:
					temp = 0; std::cin >> std::hex >> temp;
					my_testbed.start_set_loop(temp);
					break;
				case command_stoptemp:
					my_testbed.stop_loop();
					break;
				default: { };
				}
			} while (!((code_command == command_quit) || (code_command == command_exit)) );
		}
	}
	std::cin.ignore();
	std::cout << "Press enter to exit." << endl;
	std::cin.ignore(); // std::system("PAUSE");
}