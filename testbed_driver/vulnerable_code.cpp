#include "vulnerable_code.h"


namespace vulnerable_code {

		
	NTSTATUS stack_overflow_stub(IN PVOID UserBuffer, IN SIZE_T Size) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		DbgPrint(">> run_stack_overflow \r\n");

		status = run_stack_overflow(UserBuffer, Size);

		DbgPrint("<< run_stack_overflow \r\n");

		return status;
	}

	NTSTATUS run_stack_overflow(IN PVOID UserBuffer, IN SIZE_T Size) {
		ULONG KernelBuffer[BUFFER_SIZE] = { 0 }; /* sizeof = 0x800 bytes */

		RtlCopyMemory((PVOID)KernelBuffer, UserBuffer, Size);

		return STATUS_SUCCESS;
	}




	/************************************************************************/
	/* This vulnerable function has been copied from "HackSys Extreme Vulnerable Driver" 
		Module Name: StackOverflow.c
	*/
	/************************************************************************/
	/// <summary>
	/// Trigger the Stack Overflow Vulnerability
	/// </summary>
	/// <param name="UserBuffer">The pointer to user mode buffer</param>
	/// <param name="Size">Size of the user mode buffer</param>
	/// <returns>NTSTATUS</returns>
	NTSTATUS TriggerStackOverflow(IN PVOID UserBuffer, IN SIZE_T Size) {
		NTSTATUS Status = STATUS_SUCCESS;
		ULONG KernelBuffer[BUFFER_SIZE] = { 0 };

		PAGED_CODE();

		__try {
			// Verify if the buffer resides in user mode
			ProbeForRead(UserBuffer, sizeof(KernelBuffer), (ULONG)__alignof(KernelBuffer));

			DbgPrint("[+] UserBuffer: 0x%p\n", UserBuffer);
			DbgPrint("[+] UserBuffer Size: 0x%X\n", Size);
			DbgPrint("[+] KernelBuffer: 0x%p\n", &KernelBuffer);
			DbgPrint("[+] KernelBuffer Size: 0x%X\n", sizeof(KernelBuffer));

#ifdef SECURE
			// Secure Note: This is secure because the developer is passing a size
			// equal to size of KernelBuffer to RtlCopyMemory()/memcpy(). Hence,
			// there will be no overflow
			RtlCopyMemory((PVOID)KernelBuffer, UserBuffer, sizeof(KernelBuffer));
#else
			DbgPrint("[+] Triggering Stack Overflow\n");

			// Vulnerability Note: This is a vanilla Stack based Overflow vulnerability
			// because the developer is passing the user supplied size directly to
			// RtlCopyMemory()/memcpy() without validating if the size is greater or
			// equal to the size of KernelBuffer
			RtlCopyMemory( (PVOID)KernelBuffer, UserBuffer, Size);
#endif
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			Status = GetExceptionCode();
			DbgPrint("[-] Exception Code: 0x%X\n", Status);
		}

		return Status;
	}
}