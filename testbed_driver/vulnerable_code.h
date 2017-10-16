#pragma once

#include "driver.h"

namespace vulnerable_code {

	/*		This function is used to return control to DriverpDeviceControl()
	Because stack overflow overwrites return address in function run_stack_overflow()		*/
	NTSTATUS stack_overflow_stub(IN PVOID UserBuffer, IN SIZE_T Size);

	/* Run vulnerable function with memcpy() */
	NTSTATUS run_stack_overflow(IN PVOID UserBuffer, IN SIZE_T Size);

	/// <summary>
	/// Trigger the Stack Overflow Vulnerability
	/// </summary>
	/// <param name="UserBuffer">The pointer to user mode buffer</param>
	/// <param name="Size">Size of the user mode buffer</param>
	/// <returns>NTSTATUS</returns>
	NTSTATUS TriggerStackOverflow(IN PVOID UserBuffer, IN SIZE_T Size);
}