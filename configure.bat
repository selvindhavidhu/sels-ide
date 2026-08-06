@echo off
set VSLANG=1033
set VCPKG_VISUAL_STUDIO_PATH=C:\Program Files\Microsoft Visual Studio\18\Community
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake --preset=default
