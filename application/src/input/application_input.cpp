/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "application_input.hpp"

ApplicationInput::ApplicationInput() :
    InputGroup(), quitPressed(false), hideGuiPressed(false)
{
}

void ApplicationInput::onPreEventProcess()
{
    quitPressed = false;
    hideGuiPressed = false;
}

void ApplicationInput::onEventProcess(SDL_Event evt)
{
    SDL_Scancode scanCode;
    SDL_GameController *controller = nullptr;
    int playerID = 0;

    switch (evt.type)
    {
    case SDL_QUIT:
        quitPressed = true;
        break;

    case SDL_KEYDOWN:
        scanCode = evt.key.keysym.scancode;

        if (evt.key.repeat)
            break;

        switch (scanCode)
        {
        case SDL_SCANCODE_ESCAPE:
            quitPressed = true;
            break;

        case SDL_SCANCODE_F1:
            hideGuiPressed = true;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void ApplicationInput::reset()
{
    quitPressed = false;
    hideGuiPressed = false;
}
