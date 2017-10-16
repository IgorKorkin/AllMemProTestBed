REM This is a batch file to load windbg for the Windows 10 x64 for HEVD-driver

SET Win10_64_WDKPATH= "C:\Program Files (x86)\Windows Kits\10\Debuggers\x64"

SET Win10_64_workspace= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\windbgbats\config2.wew"

SET Win10_64_ImagePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug"

SET Win10_64_SymbolPath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug;C:\symbols\10x64;SRV*C:\symbols\10x64*http://msdl.microsoft.com/download/symbols"

SET Win10_64_SourcePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed"

SET Win10_64_ComPort=\\.\pipe\com_10_64

REM **********************************/
REM This is a batch file to load windbg 

C:
cd %Win10_64_WDKPATH%

start windbg.exe -QY -QSY -WF %Win10_64_workspace% -i %Win10_64_ImagePath% -y %Win10_64_SymbolPath% -srcpath %Win10_64_SourcePath% -k com:pipe,port=%Win10_64_ComPort%,resets=0,reconnect

REM windbg.exe -W TestBed -i  "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug" -y  "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug;K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\testbed_console\x64\Debug;C:\symbols\10x64;SRV*C:\symbols\10x64*http://msdl.microsoft.com/download/symbols" -srcpath  "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed" -k com:pipe,port=\\.\pipe\com_10_64,resets=0,reconnect

REM .reload