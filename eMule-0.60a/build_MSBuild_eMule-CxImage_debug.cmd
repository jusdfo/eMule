@ECHO OFF

CD /D %~dp0

CALL incl_VCVARS64.cmd

CD eMule\CxImage\CxImage

MSBuild cximage.vcxproj -target:Clean,Build /property:Configuration=Debug /property:Platform=x64

IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  CD /D %~dp0
  DEL /Q eMule\Libs_Debug\cximage*
  COPY eMule\CxImage\CxImage\x64\Debug\CxImage.lib            eMule\Libs_Debug\
  COPY eMule\CxImage\CxImage\x64\Debug\CxImage.pdb            eMule\Libs_Debug\
)
