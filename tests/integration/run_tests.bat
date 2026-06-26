@echo off
setlocal enabledelayedexpansion
set "PARSER=..\..\build\KNPO_Java_Parser.exe"
set "OUT_DIR=.\outputs"
set "LOG_DIR=.\logs"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

:: Основные тесты T*.java
for %%f in (.\inputs\T*.java) do (
    set "name=%%~nf"
    echo Running !name!...
    "%PARSER%" "%%f" "%OUT_DIR%\!name!"
)

:: T27 — нет аргументов
echo T27_NoArgs
"%PARSER%" 2>"%LOG_DIR%\T27_NoArgs.log"
echo Exit code: %errorlevel%

:: T28 — только входной файл
echo T28_OnlyInput
"%PARSER%" .\inputs\T1.java 2>"%LOG_DIR%\T28_OnlyInput.log"
echo Exit code: %errorlevel%

:: T29 — лишний аргумент
echo T29_ExtraArgs
"%PARSER%" .\inputs\T1.java .\outputs\T1 extra 2>"%LOG_DIR%\T29_ExtraArgs.log"
echo Exit code: %errorlevel%

:: T30 — несуществующий файл
echo T30_MissingFile
"%PARSER%" .\inputs\DOES_NOT_EXIST.java .\outputs\test 2>"%LOG_DIR%\T30_MissingFile.log"
echo Exit code: %errorlevel%

:: T31 — пустой файл
echo T31_EmptyFile
type nul > .\inputs\empty.java
"%PARSER%" .\inputs\empty.java .\outputs\test 2>"%LOG_DIR%\T31_EmptyFile.log"
echo Exit code: %errorlevel%

:: T32 — файл без прав на чтение
echo T32_NoPermissionFile
type nul > .\inputs\noperm.java
icacls .\inputs\noperm.java /deny "%USERNAME%":R >nul 2>&1
"%PARSER%" .\inputs\noperm.java .\outputs\test 2>"%LOG_DIR%\T32_NoPermissionFile.log"
echo Exit code: %errorlevel%
icacls .\inputs\noperm.java /remove:d "%USERNAME%" >nul 2>&1

echo Done.
endlocal
