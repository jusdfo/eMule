@REM Post-Build Command
@REM call  $(SolutionDir)batch\post-build-event.bat $(SolutionDir)$(OutputPath) D:\program1\downloader\eMule
xcopy /exclude:exclude.txt %1* %2 /S /Y