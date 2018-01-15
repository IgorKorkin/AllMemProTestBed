#pragma once 

#include "driver.h"
#include "asm.h"


typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemUnused1,
	SystemPerformanceTraceInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemUnused3,
	SystemUnused4,
	SystemUnused5,
	SystemUnused6,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation,
	SystemTimeSlipNotification,
	SystemSessionCreate,
	SystemSessionDetach,
	SystemSessionInformation

} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;


typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	ULONG PrivatePageCount;
	VM_COUNTERS VirtualMemoryCounters;
	IO_COUNTERS IoCounters;
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _SYSTEM_BASIC_INFORMATION { // Information Class 0 
	ULONG Unknown;
	ULONG MaximumIncrement;
	ULONG PhysicalPageSize;
	ULONG NumberOfPhysicalPages;
	ULONG LowestPhysicalPage;
	ULONG HighestPhysicalPage;
	ULONG AllocationGranularity;
	ULONG LowestUserAddress;
	ULONG HighestUserAddress;
	ULONG ActiveProcessors;
	UCHAR NumberProcessors;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_INFORMATION { // Information Class 1 
	USHORT ProcessorArchitecture;
	USHORT ProcessorLevel;
	USHORT ProcessorRevision;
	USHORT Unknown;
	ULONG FeatureBits;
} SYSTEM_PROCESSOR_INFORMATION, *PSYSTEM_PROCESSOR_INFORMATION;


extern "C"
{
	NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
	);
};


extern "C" namespace basic_mem_access {

	/*Testbed structure to play with memory accesses*/
	
	typedef struct _TESTBED_STR {
		unsigned char one_byte;
		char buf[20];
	}TESTBED_STR, *PTESTBED_STR;

	/* Simple global structure*/
	extern TESTBED_STR _GlobalStruct;

	/*  */
	typedef struct _REACTOR_CONFIG
	{
		ULONG64 param1;
		ULONG64 tempReactor; // temperature of nuclear reactor
		ULONG64 param3;
		ULONG64 param4;
		void* buf_proc_info;
		ULONG buf_proc_info_sz;
	}REACTOR_CONFIG;

	/*  */
	typedef struct _CONFIG_THREAD {
		bool flagLoopIsActive;
		HANDLE handleMemoryLoop;
		PKTHREAD pthread;
		void * pconfig_data; /* PREACTOR_CONFIG pinit_data */
		KMUTEX mutex;
	}CONFIG_THREAD, *PCONFIG_THREAD;

	/*  */
	KSTART_ROUTINE memory_access_loop;

	/*  */
	extern CONFIG_THREAD configThread;

	class BasicMemoryAccess {
	public:
		/* allocate variables */
		void init();

		/* Run basic memory accesses: setting and printing variables*/
		NTSTATUS basic_memory_accesses();

		/*  */
		NTSTATUS start_set_thread(void* inbuf, void *outbuf);

		/*  */
		NTSTATUS stop_thread();

		/*  */
		NTSTATUS set_memory_thread(void* inbuf, void *outbuf);

	private:
		/* Pointer to the structure in the PagedPool memory */
		PTESTBED_STR _pGlobalStrPaged;

		/* Pointer to the structure in the NonPagedPool memory */
		PTESTBED_STR _pGlobalStrNonPaged;

		/* Print the content of the structure */
		void print_struct(const char* title, const TESTBED_STR * str);
	};
	


}