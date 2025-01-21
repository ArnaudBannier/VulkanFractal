rem Use this batch file to build for Visual Studio
rmdir /s /q _build
mkdir _build
cd _build
cmake ..
rem cmake --build .
