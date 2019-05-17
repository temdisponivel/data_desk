@echo off
start /b /wait "" "cl.exe" -nologo /Zi -I ..\..\source\ enums_and_flags.c /LD
data_desk --custom enums_and_flags.dll enums_and_flags.ds