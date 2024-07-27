if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set target="%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" 
) else (
    if exist "D:\program1\IDE\vs\VC\Auxiliary\Build\vcvars64.bat" (
        set target="D:\program1\IDE\vs\VC\Auxiliary\Build\vcvars64.bat" 
    ) else (
        if exist "E:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
            set target="E:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 
        )
    )
)
@REM echo %target%
CALL %target%
