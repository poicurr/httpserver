@echo off

cmake -S . -B build -DHTTP_SERVER_BUILD_EXAMPLES=ON
cmake --build build --config Release

