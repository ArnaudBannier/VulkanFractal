/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "vulkan/ve_window.hpp"

Window::Window(int width, int height, std::string name) :
    m_width{static_cast<uint32_t>(width)},
    m_height{static_cast<uint32_t>(height)},
    m_name{name}
{
    initWindow();
}

Window::~Window()
{
    SDL_DestroyWindow(m_window);
}

void Window::createSurface(vk::Instance instance, vk::SurfaceKHR *surface)
{
    VkSurfaceKHR cSurface{};
    if (SDL_Vulkan_CreateSurface(m_window, instance, &cSurface) == 0)
    {
        printf("Failed to create Vulkan surface\n");
        assert(false); abort();
    }
    *surface = cSurface;
}

void Window::initWindow()
{
    Uint32 windowsFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
#ifdef FULLSCREEN
    windowsFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
    m_window = SDL_CreateWindow(
        m_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height, windowsFlags
    );

    if (m_window == nullptr)
    {
        printf("ERROR - Create window %s\n", SDL_GetError());
        assert(false); abort();
    }
}

void Window::update()
{
    int w, h;
    SDL_Vulkan_GetDrawableSize(m_window, &w, &h);

    m_isResized = m_isResized || (m_width != w) || (m_height != h);
    m_width = static_cast<uint32_t>(w);
    m_height = static_cast<uint32_t>(h);
}
