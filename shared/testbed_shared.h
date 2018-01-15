#ifndef __TESTBED_SHARED_H__
#define __TESTBED_SHARED_H__

#define		__TESTBED__					L"TestBedDriver"
#define		TESTBED_SYS_FILE				__TESTBED__ \
											L".sys"
// There are symbols for driver
#define		TESTBED_DEVICENAME_DRV	L"\\Device\\dev" __TESTBED__
#define		TESTBED_LINKNAME_DRV 	L"\\DosDevices\\" __TESTBED__

// There are symbols for command line app
#define		TESTBED_LINKNAME_APP 	L"\\\\.\\" __TESTBED__
#define		TESTBED_SERVNAME_APP	__TESTBED__


// Device type in user defined range
#define TESTBED_DEVICE_IOCTL  0x8301
#define TESTBED_BASIC_MEMORY_ACCESS			(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE 512
#define TESTBED_SIMPLE_STACK_OVERFLOW		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x810, METHOD_NEITHER, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////

#define TESTBED_SIMPLE_POOL_OVERFLOW		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x820, METHOD_NEITHER, FILE_ANY_ACCESS)


//////////////////////////////////////////////////////////////////////////
namespace payload_use_after_free {
	const int g_Objectsz = 0x54;
	typedef struct _BUFFER_OBJECT {
		char buffer[g_Objectsz];
	} BUFFER_OBJECT, *PBUFFER_OBJECT;

	typedef void(*FunctionPointer)();
	typedef struct _BUFFER_FUNC {
		FunctionPointer callback_func; // ! it must be always in the first place
		BUFFER_OBJECT object;
	} BUFFER_FUNC, *PBUFFER_FUNC;
}


#define TESTBED_UAF_ALLOCATE_OBJECT	(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x830, METHOD_NEITHER, FILE_ANY_ACCESS)
#define TESTBED_UAF_FREE_OBJECT		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x830+1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define TESTBED_UAF_USE_OBJECT		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x830+2, METHOD_NEITHER, FILE_ANY_ACCESS)
#define TESTBED_UAF_ALLOCATE_FAKE	(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x830+3, METHOD_NEITHER, FILE_ANY_ACCESS)

#define TESTBED_START_SET_THREAD		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x840, METHOD_NEITHER, FILE_ANY_ACCESS)
#define TESTBED_STOP_THREAD			(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x840+1, METHOD_NEITHER, FILE_ANY_ACCESS)

#endif // __TESTBED_SHARED_H__