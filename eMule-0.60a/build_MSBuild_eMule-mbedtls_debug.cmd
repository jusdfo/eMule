@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\mbedtls\visualc\VS2010

MSBuild mbedTLS.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\mbedTLS*
  COPY eMule\mbedtls\visualc\VS2010\x64\Debug\mbedTLS.lib     eMule\Libs_Debug\
  COPY eMule\mbedtls\visualc\VS2010\x64\Debug\mbedTLS.pdb     eMule\Libs_Debug\
)
