@echo off
if not exist .\build mkdir build
if exist .\build\CMakeCache.txt del .\build\CMakeCache.txt

cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
