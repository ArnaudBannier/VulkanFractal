# Mandelbrot and Julia Fractals in Vulkan

This project corresponds to a practical course given at ESIEA. It allows you to visualize both Mandelbrot and Julia sets with an application written in C/C++. The graphics engine uses Vulkan to exploit the power of the GPU.

## Building for Visual Studio (Windows)

This project was developed for Windows under Visual Studio. Although the code should easily be ported to other platforms, no testing has been carried out.
To compile this project, make sure you have correctly installed the following tools:
- [Visual Studio](https://visualstudio.microsoft.com/) with "Desktop Development in C++" option
- [Vulkan SDK](https://vulkan.lunarg.com/)
- [CMake](https://cmake.org/download/)

Then run the build.bat script. The Visual Studio solution is created in the ``_build`` folder.

## Licence

All code produced for this project is licensed under the [MIT license](/LICENSE.md). It uses the follwing libraries:
[ImGUI](https://github.com/ocornut/imgui), 
[GLM](https://github.com/g-truc/glm), 
[SDL2](https://github.com/libsdl-org/SDL), 
[STB_image](https://github.com/nothings/stb), 
[Vulkan HPP](https://github.com/KhronosGroup/Vulkan-Hpp).

The Vulkan Engine project is inspired by the amazing [Vulkan Tutorial](https://github.com/Overv/VulkanTutorial), [Sascha Willems](https://github.com/SaschaWillems/Vulkan) examples and the [Vookoo](https://github.com/andy-thomason/Vookoo) library.