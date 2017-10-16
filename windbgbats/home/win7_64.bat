REM **********************************/
REM **********************************/
REM This is a batch file to load windbg for the Windows 7 x64 for HEVD-driver

SET Win7_64_WDKPATH= "C:\WinDDK\7600.16385.1\Debuggers\"

SET Win7_64_workspace= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\windbgbats\config2.wew"

SET Win7_64_ImagePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug"

SET Win7_64_SymbolPath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed\x64\Debug;D:\symbols\64\7;SRV*D:\symbols\64\7*http://msdl.microsoft.com/download/symbols"

SET Win7_64_SourcePath= "K:\Dropbox\_Pubs&Confs\18\Active Memory Protection\TestBed"

SET Win7_64_ComPort=\\.\pipe\com_7_x64

REM **********************************/
REM This is a batch file to load windbg 

C:
cd %Win7_64_WDKPATH%

start windbg.exe -QY -QSY -WF %Win7_64_workspace% -i %Win7_64_ImagePath% -y %Win7_64_SymbolPath% -srcpath %Win7_64_SourcePath% -k com:pipe,port=%Win7_64_ComPort%,resets=0,reconnect