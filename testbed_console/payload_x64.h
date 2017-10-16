
#ifndef  __PAYLOAD__X64_H__
#define  __PAYLOAD__X64_H__

extern "C" 
{
	/*!!!
	Payload only for
		Windows 7 Kernel Version 15063 MP (1 procs) Free x64
		Product: WinNt, suite: TerminalServer SingleUserTS
		Built by: 15063.0.amd64fre.rs2_release.170317-1834
	*/

	void TokenStealingPayloadWin10();

	const DWORD g_dwDefaultPid = 0xDDAABBEE;
	const byte g_bDefaultPid[] = "\xEE\xBB\xAA\xDD"; // g_bDefaultPid = 0DDAABBEEh
}

#endif // __PAYLOAD__X64_H__