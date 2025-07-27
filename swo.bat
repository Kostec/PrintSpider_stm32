@echo off
set host=localhost
set port=3344
chcp 65001
cls

:loop
  :: check if port available
  >nul 2>&1 (echo >\\.\%host%:%port%)
  if errorlevel 1 (
    echo Port %port% is unavailable — wait...
    timeout /T 2 /NOBREAK >nul
  ) else (
    %1 -raw %host% -P %port%
  )
goto loop
