@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\libpng\projects\vstudio

MSBuild vstudio.sln -target:Clean,Build /property:Configuration="Debug Library" /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q libs_debug\libpng15*
    DEL /Q libs_debug\libpng*
  COPY "eMule\libpng\projects\vstudio\x64\Debug Library\libpng15.lib"    eMule\Libs_Debug\
  COPY "eMule\libpng\projects\vstudio\x64\Debug Library\libpng15.pdb"    eMule\Libs_Debug\
  REN eMule\Libs_Debug\libpng15.lib libpng.lib
  REN eMule\Libs_Debug\libpng15.pdb libpng.pdb
  COPY "eMule\libpng\projects\vstudio\x64\Debug Library\libpng.lib"    eMule\Libs_Debug\
  COPY "eMule\libpng\projects\vstudio\x64\Debug Library\libpng.pdb"    eMule\Libs_Debug\
)
