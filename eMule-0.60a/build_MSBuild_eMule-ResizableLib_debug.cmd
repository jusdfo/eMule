@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\ResizableLib\ResizableLib

MSBuild ResizableLib.vcxproj -target:Clean,Build /property:Configuration="Debug Static" /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\ResizableLib*
  COPY "eMule\ResizableLib\ResizableLib\x64\Debug Static\ResizableLib.lib"            eMule\Libs_Debug\
  COPY "eMule\ResizableLib\ResizableLib\x64\Debug Static\ResizableLib.pdb"            eMule\Libs_Debug\
)
