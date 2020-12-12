@echo off

set compile_flags= /nologo /Zi /FC /MP /I..\source\ -DDD_WIN32=1

if not exist build mkdir build
pushd build
cl %compile_flags% ..\samples\old_style_custom_layer.c
cl %compile_flags% ..\samples\parsing_test.c
cl %compile_flags% ..\samples\c_passthrough.c
cl %compile_flags% ..\samples\static_site_generator.c
cl %compile_flags% ..\tests\unicode_test.c
popd

pushd samples
pushd example_site
if not exist generated mkdir generated
pushd generated
..\..\..\build\static_site_generator.exe --siteinfo ..\site_info.dd --pagedir ..\
popd
popd
popd