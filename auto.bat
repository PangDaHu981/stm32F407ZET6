@echo off
gcc -O0 auto.c -o auto
auto p "Version.h" "10_Project\\Project IAR8.3.ewp" "PROJECT_NAME" "SYSTEM_VERSION"
del ".\\10_Project\\Project IAR8.3.dep"
del auto.exe
start clean.bat