/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

//#define FHD
//#define FULLSCREEN

#define HD_WIDTH 1280
#define HD_HEIGHT 720
#define FHD_WIDTH 1920
#define FHD_HEIGHT 1080

#ifdef FHD
#define WINDOW_WIDTH   FHD_WIDTH
#define WINDOW_HEIGHT  FHD_HEIGHT
#define LOGICAL_WIDTH  FHD_WIDTH
#define LOGICAL_HEIGHT FHD_HEIGHT
#else
#define WINDOW_WIDTH   HD_WIDTH
#define WINDOW_HEIGHT  HD_HEIGHT
#define LOGICAL_WIDTH  HD_WIDTH
#define LOGICAL_HEIGHT HD_HEIGHT
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ve.hpp"
#include "input/mouse_input.hpp"
#include "input/application_input.hpp"
#include "application.hpp"

int main(int argc, char *argv[])
{
    //--------------------------------------------------------------------------
    // Initialisation

    // Initialisation de la SDL
    Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    if (SDL_Init(sdlFlags) < 0)
    {
        printf("ERROR - SDL_Init %s\n", SDL_GetError());
        assert(false);
        abort();
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    //--------------------------------------------------------------------------
    // Application

    Window m_window{ WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan fractal" };

    InstanceBuilder instanceBuilder;
    instanceBuilder
        .addSDLExtensions(m_window.getSDL())
        .enableValidation();

    DeviceBuilder deviceBuilder;
    deviceBuilder
        .addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
        .enableGeometryShader()
        .enableSamplerAnisotropy()
        .enableShaderFloat64();

    DescriptorPoolBuilder descriptorPoolBuilder;
    descriptorPoolBuilder
        .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .addPoolSize(vk::DescriptorType::eUniformBuffer, 10 * Renderer::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(vk::DescriptorType::eCombinedImageSampler, 4 * Renderer::MAX_FRAMES_IN_FLIGHT);

    try
    {
        Framework framework(instanceBuilder, deviceBuilder, descriptorPoolBuilder, m_window);
        Application app(framework);

        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    SDL_Quit();

    return EXIT_SUCCESS;
}
