@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\zlib\contrib\vstudio\vc17

MSBuild zlibstat.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\zlibstat*
  COPY eMule\zlib\contrib\vstudio\vc17\x64\ZlibStatDebug\zlibstat.lib              eMule\Libs_Debug\
  COPY eMule\zlib\contrib\vstudio\vc17\x64\ZlibStatDebug\zlibstat.pdb              eMule\Libs_Debug\
  REN eMule\Libs_Debug\zlibstat.lib zlib.lib
  REN eMule\Libs_Debug\zlibstat.pdb zlib.pdb
  COPY eMule\zlib\contrib\vstudio\vc17\x64\ZlibStatDebug\zlibstat.lib              eMule\Libs_Debug\
  COPY eMule\zlib\contrib\vstudio\vc17\x64\ZlibStatDebug\zlibstat.pdb              eMule\Libs_Debug\
)
