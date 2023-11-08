# Chorus Game Engine
This is a small game engine that I am building using the Vulkan Graphics API.  
The goal is to somewhat emulate what I have done in WebGL on my Envy rendering engine, but there is a lot more overhead compared to javascript and even compared to OpenGL itself.  

## Dependencies
- GLFW: used for cross platform windowing
- GLM: linear algebra library
- GLSLC: library for compiling shaders. The `makefile` has a path to where mine is locally, but you can change it to whatever path you want
- clang: c++ compiler

## Usage
The current build system is `Make`, but windows `Make` also works. You can build using `make` or `make all`. This will compile the shaders as well as the source code.     
You can run the binary directly, or you can also use `make run`. To clean, you can run `make clean` which will empty the bin folder and delete the compiled shaders.

## Current Features
- Custom object loading
- 3D camera movement (WASD) Space/Shift
- Lighting

## Features In Progress
- Multipoint lighting
- Phong lighting
- Multiple Objects
- Materials
