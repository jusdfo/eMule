@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\id3lib\libprj

MSBuild id3lib.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\id3lib*
  COPY eMule\id3lib\libprj\x64\Debug\id3lib.lib              eMule\Libs_Debug\
  COPY eMule\id3lib\libprj\x64\Debug\id3lib.pdb              eMule\Libs_Debug\
)
