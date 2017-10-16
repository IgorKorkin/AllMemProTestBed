

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

#define BUFFER_SIZE 512
#define TESTBED_SIMPLE_STACK_OVERFLOW		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)

#define TESTBED_STACK_OVERFLOW_FROM_HEVD		(unsigned) CTL_CODE(TESTBED_DEVICE_IOCTL, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)