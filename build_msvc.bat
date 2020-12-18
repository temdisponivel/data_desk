@echo off
if not exist build mkdir build
pushd build
cl -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\source\data_desk_main.c /link /out:data_desk.exe
cl -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 ..\source\custom_layer.c /link /DLL /out:custom_layer.dll
popd