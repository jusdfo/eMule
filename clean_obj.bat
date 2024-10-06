@echo off
set nowPath=%cd%
cd /
cd %nowPath%

::delete specify file(*.pdb,*.vshost.*)
for /r %nowPath% %%i in (*.pdb,*.obj,*.ipdb,*.tlog,*.pch,*.ilk) do (
    echo %%i
    del "%%i")

::delete specify folder(obj,bin,.vs)
for /r %nowPath% %%i in (ipch) do (IF EXIST %%i RD /s /q %%i)

echo OK
pause
