@echo off

set compile_flags= /nologo /Zi /I..\source\ -DDD_WIN32=1

if not exist build mkdir build
pushd build
cl %compile_flags% ..\samples\old_style_custom_layer.c
cl %compile_flags% ..\samples\parsing_test.c
popd