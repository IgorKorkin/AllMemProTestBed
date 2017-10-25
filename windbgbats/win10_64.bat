REM **********************************/
REM **********************************/
REM This is a batch file to load windbg for the Windows 10 x64 for HEVD-driver

SET Win10_64_WDKPATH= "C:\WinDDK\7600.16385.1\Debuggers\"

SET Win10_64_workspace= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\windbgbats\config2.wew"

SET Win10_64_ImagePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug"

SET Win10_64_SymbolPath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug;D:\symbols\64\10;SRV*D:\symbols\64\10*http://msdl.microsoft.com/download/symbols"

SET Win10_64_SourcePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed"

SET Win10_64_ComPort=\\.\pipe\com_10_64

REM **********************************/
REM This is a batch file to load windbg 

C:
cd %Win10_64_WDKPATH%

start windbg.exe -QY -QSY -WF %Win10_64_workspace% -i %Win10_64_ImagePath% -y %Win10_64_SymbolPath% -srcpath %Win10_64_SourcePath% -k com:pipe,port=%Win10_64_ComPort%,resets=0,reconnect

REM windbg.exe -W TestBed -i  "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug" -y "k:\dropbox\_pubs&confs\18\active memory protection\testbed\testbed_console;K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug;K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\testbed_driver\x64\Debug;K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\testbed_console\x64\Debug;D:\symbols\64\10;SRV*D:\symbols\64\10*http://msdl.microsoft.com/download/symbols" -srcpath  "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed" -k com:pipe,port=\\.\pipe\com_10_64,resets=0,reconnect