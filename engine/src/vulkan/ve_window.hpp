/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"

class Window
{
public:
    Window(int width, int height, std::string name);
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    bool isResized() const;
    void resetIsResized();

    SDL_Window *getSDL() const;
    vk::Extent2D getExtent() const;

    void createSurface(vk::Instance instance, vk::SurfaceKHR *surface);

    bool isMinimized() const;
    bool isMaximized() const;

    void update();

private:
    std::string m_name;
    uint32_t m_width;
    uint32_t m_height;
    bool m_isResized = false;

    SDL_Window *m_window;

    void initWindow();
};

inline bool Window::isResized() const
{
    return m_isResized;
}

inline void Window::resetIsResized()
{
    m_isResized = false;
}

inline SDL_Window *Window::getSDL() const
{
    return m_window;
}

inline vk::Extent2D Window::getExtent() const
{
    vk::Extent2D extent = { m_width, m_height };
    return extent;
}

inline bool Window::isMinimized() const
{
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return (flags & SDL_WINDOW_MINIMIZED) != 0;
}

inline bool Window::isMaximized() const
{
    Uint32 flags = SDL_GetWindowFlags(m_window);
    return (flags & SDL_WINDOW_MAXIMIZED) != 0;
}
