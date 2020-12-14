@echo off

set compile_flags= /nologo /Zi /FC /MP /I..\source\ -DDD_WIN32=1

if not exist build mkdir build
pushd build
cl %compile_flags% ..\samples\old_style_custom_layer.c
cl %compile_flags% ..\samples\static_site_generator\static_site_generator.c
cl %compile_flags% ..\tests\sanity_tests.c
cl %compile_flags% ..\tests\unicode_test.c
cl %compile_flags% ..\tests\cpp_build_test.cpp
popd

pushd samples
pushd static_site_generator
pushd example_site
if not exist generated mkdir generated
pushd generated
..\..\..\..\build\static_site_generator.exe --siteinfo ..\site_info.md --pagedir ..\
popd
popd
popd
popd