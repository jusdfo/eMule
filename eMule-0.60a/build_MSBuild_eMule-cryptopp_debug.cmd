@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\cryptopp

MSBuild cryptlib.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\cryptlib*
  COPY eMule\cryptopp\x64\Output\Debug\cryptlib.lib          eMule\Libs_Debug\
  COPY eMule\cryptopp\x64\Output\Debug\cryptlib.pdb          eMule\Libs_Debug\
)
