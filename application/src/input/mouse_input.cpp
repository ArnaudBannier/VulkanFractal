/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "mouse_input.hpp"

MouseInput::MouseInput() :
    InputGroup(),
    leftClickCount(0), leftDown(false), leftReleased(false),
    rightClickCount(0), rightDown(false), rightReleased(false),
    middleClick(false), wheel(0)
{
    viewPos = glm::vec2(0.f);
    lastViewPos = glm::vec2(0.f);
}

void MouseInput::onPreEventProcess()
{
    leftClickCount = 0;
    leftReleased = false;
    rightClickCount = 0;
    rightReleased = false;
    middleClick = false;
    wheel = 0;
    lastViewPos = viewPos;
}

void MouseInput::onEventProcess(SDL_Event evt)
{
    Uint8 mouseButton;
    switch (evt.type)
    {
    case SDL_MOUSEMOTION:
        viewPos = glm::vec2(
            static_cast<float>(evt.motion.x),
            static_cast<float>(evt.motion.y));
        break;

    case SDL_MOUSEBUTTONDOWN:
        mouseButton = evt.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_MIDDLE:
            middleClick = true;
            break;
        case SDL_BUTTON_LEFT:
            leftClickCount = evt.button.clicks;
            leftDown = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightClickCount = evt.button.clicks;
            rightDown = true;
            break;
        default:
            break;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        mouseButton = evt.button.button;

        switch (mouseButton)
        {
        case SDL_BUTTON_LEFT:
            leftDown = false;
            leftReleased = true;
            break;
        case SDL_BUTTON_RIGHT:
            rightDown = false;
            rightReleased = true;
            break;
        default:
            break;
        }
        break;

    case SDL_MOUSEWHEEL:
        wheel += evt.wheel.y;
        break;

    default:
        break;
    }
}

void MouseInput::onPostEventProcess()
{
    deltaPos = viewPos - lastViewPos;
}

void MouseInput::reset()
{
    viewPos = glm::vec2(0.f);
    lastViewPos = glm::vec2(0.f);

    leftClickCount = 0;
    leftDown = false;
    leftReleased = false;

    rightClickCount = 0;
    rightDown = false;
    rightReleased = false;

    middleClick = false;
    wheel = 0;
}
