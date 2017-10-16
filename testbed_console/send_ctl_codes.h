#ifndef  __SEND_STL_CODES_H__
#define  __SEND_STL_CODES_H__

#include "print_messages.h" // print :: print_mes, print :: print_last_err

#include "service_functions.h"
#include "..\shared\testbed_shared.h"
#include "payload_stack_overflow.h"

namespace send_ctl_codes
{
	extern service_functions :: ServiceManager m_smCsmgr;

	bool run_basic_mem_access();

	bool run_simple_stack_overflow(DWORD bufferSz);

	bool run_stack_overflow_with_payload(DWORD targetPid);

}
#endif  //  __SEND_STL_CODES_H__