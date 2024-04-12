@ECHO OFF

REM Compile Shaders
glslc -fshader-stage=vertex ./assets/shaders/triangle.vert.glsl -o ./assets/shaders/triangle.vert.spv
glslc -fshader-stage=fragment ./assets/shaders/triangle.frag.glsl -o ./assets/shaders/triangle.frag.spv

REM Set Build Parameters
SET COMPILER=clang
SET ROOT_SRC_FILE=.\src\main.c
SET OUTPUT_ASSEMBLY=vulkan_renderer.exe
SET BIN_DIR=.\bin
SET INCLUDE_DIRS=-Iinclude -I%VULKAN_SDK%\Include
SET LIB_DIRS=-L.\lib\GLFW
SET LINKED_LIBS=-lglfw3
SET COMPILER_FLAGS=-g -gcodeview -O0 -MD
SET LINKER_FLAGS=-Wl,--pdb=
SET DEFINES=-D_DEBUG -DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DVK_USE_PLATFORM_WIN32_KHR

REM Clean
rm -f %BIN_DIR%/*

REM Build
%COMPILER% %ROOT_SRC_FILE% -o %BIN_DIR%/%OUTPUT_ASSEMBLY% %DEFINES% %INCLUDE_DIRS% %COMPILER_FLAGS% %LIB_DIRS% %LINKED_LIBS% %LINKER_FLAGS%

REM Output Result
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit) ELSE (echo "Build succeeded!")