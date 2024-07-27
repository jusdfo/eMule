@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\miniupnp\miniupnpc\msvc

MSBuild miniupnpc.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\miniupnpc*
  COPY eMule\miniupnp\miniupnpc\msvc\x64\Debug\miniupnpc.lib              eMule\Libs_Debug\
  COPY eMule\miniupnp\miniupnpc\msvc\x64\Debug\miniupnpc.pdb              eMule\Libs_Debug\
)
