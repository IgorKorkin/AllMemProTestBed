#pragma once 

#include "driver.h"

extern "C" namespace basic_mem_access {

	/*Testbed structure to play with memory accesses*/
	
	typedef struct _TESTBED_STR {
		unsigned char one_byte;
		char buf[20];
	}TESTBED_STR, *PTESTBED_STR;

	/* Simple global structure*/
	extern TESTBED_STR _GlobalStruct;
	
	class BasicMemoryAccess {
	public:
		/* allocate variables */
		void init();

		/* Run basic memory accesses: setting and printing variables*/
		NTSTATUS basic_memory_accesses();

	private:
		/* Pointer to the structure in the PagedPool memory */
		PTESTBED_STR _pGlobalStrPaged;

		/* Pointer to the structure in the NonPagedPool memory */
		PTESTBED_STR _pGlobalStrNonPaged;

		/* Print the content of the structure */
		void print_struct(const char* title, const TESTBED_STR * str);
	};
	


}