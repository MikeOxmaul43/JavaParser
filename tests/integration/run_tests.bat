@echo off
setlocal enabledelayedexpansion

set "PARSER=..\..\build\KNPO_Java_Parser.exe"
set "OUT_DIR=.\outputs"

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

for %%f in (.\inputs\T*.java) do (
    set "name=%%~nf"
    echo Running !name!...
    "%PARSER%" "%%f" "%OUT_DIR%\!name!"
)

echo Done.
endlocal
