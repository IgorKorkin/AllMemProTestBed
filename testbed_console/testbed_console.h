
#include "windows.h"
#include "windef.h"
#include "stdlib.h"
#include "stdio.h" // sscanf_s, _snscanf_s
#include "Ntsecapi.h" // UNICODE_STRING
#include "resource.h"
#include "Shlwapi.h" // PathFileExists
#pragma comment(lib,"shlwapi.lib") // PathFileExists

#include "print_messages.h"
#include "send_ctl_codes.h"
#include "..\shared\testbed_shared.h"


namespace testbed_console {

	bool activate_testbed();

	void close_stop_remove();

}

extern "C"
{
	NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(
		__in PCWSTR DosFileName,
		__out UNICODE_STRING *NtFileName,
		__out_opt PWSTR *FilePart,
		__out_opt PVOID RelativeName
		);

#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_AMBIGUOUS   (0x00000001)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_UNC         (0x00000002)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_DRIVE       (0x00000003)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_ALREADY_DOS (0x00000004)

	typedef struct _RTL_BUFFER {
		PWCHAR        Buffer;
		PWCHAR        StaticBuffer;
		SIZE_T    Size;
		SIZE_T    StaticSize;
		SIZE_T    ReservedForAllocatedSize; // for future doubling
		PVOID     ReservedForIMalloc; // for future pluggable growth
	} RTL_BUFFER, *PRTL_BUFFER;

	typedef struct _RTL_UNICODE_STRING_BUFFER {
		UNICODE_STRING String;
		RTL_BUFFER     ByteBuffer;
		UCHAR          MinimumStaticBufferForTerminalNul[sizeof(WCHAR)];
	} RTL_UNICODE_STRING_BUFFER, *PRTL_UNICODE_STRING_BUFFER;

	NTSYSAPI NTSTATUS NTAPI RtlNtPathNameToDosPathName(
			__in ULONG Flags,
			__inout PRTL_UNICODE_STRING_BUFFER Path,
			__out_opt PULONG Disposition,
			__inout_opt PWSTR* FilePart
			);

};
