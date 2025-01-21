# Mandelbrot and Julia Fractals in Vulkan

This project corresponds to a practical course given at ESIEA. It allows you to visualize both Mandelbrot and Julia sets with an application written in C/C++. The graphics engine uses Vulkan to exploit the power of the GPU.

## Building for Visual Studio (Windows)

This project was developed for Windows under Visual Studio. Although the code should easily be ported to other platforms, no testing has been carried out.
To compile this project, make sure you have correctly installed the following tools:
- [Visual Studio](https://visualstudio.microsoft.com/) with "Desktop Development in C++" option
- [Vulkan SDK](https://vulkan.lunarg.com/)
- [CMake](https://cmake.org/download/)

Rename the ``build_vs.txt`` file to ``build_vs.bat`` and double-click on this file to run the script.
The Visual Studio solution is created in the ``_build_vs`` folder.

If you wish to compile the project manually, you can run the following commands:
```
mkdir _build_vs
cd _build_vs
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Licence

All code produced for this project is licensed under the [MIT license](/LICENSE.md). It uses the follwing libraries:
[ImGUI](https://github.com/ocornut/imgui), 
[GLM](https://github.com/g-truc/glm), 
[SDL2](https://github.com/libsdl-org/SDL), 
[STB_image](https://github.com/nothings/stb), 
[Vulkan HPP](https://github.com/KhronosGroup/Vulkan-Hpp).

The Vulkan Engine project is inspired by the amazing [Vulkan Tutorial](https://github.com/Overv/VulkanTutorial), [Sascha Willems](https://github.com/SaschaWillems/Vulkan) examples and the [Vookoo](https://github.com/andy-thomason/Vookoo) library.