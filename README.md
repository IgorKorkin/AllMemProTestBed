# testbed
This project demonstrates the privilege escalation for a user-mode process - cmd.exe using stack overflow in the kernel mode driver.

The user-mode component 'testbed_console.exe' sends CTL_CODE with a payload to the vulnerable driver 'testbed_driver.sys', which call RtlCopyMemory without any checks.
'testbed_console.exe' includes 'testbed_driver.sys' has a resource.
