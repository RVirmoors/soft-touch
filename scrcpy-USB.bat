@echo off
scrcpy.exe -d --window-x=1400
:: if the exit code is >= 1, then pause
if errorlevel 1 pause
