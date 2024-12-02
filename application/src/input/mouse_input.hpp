/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve.hpp"

class MouseInput : public InputGroup
{
public:
    MouseInput();

    virtual void onPreEventProcess() override;
    virtual void onEventProcess(SDL_Event evt) override;
    virtual void onPostEventProcess() override;
    virtual void reset() override;

    // Position de la souris
    glm::vec2 viewPos;
    glm::vec2 lastViewPos;
    glm::vec2 deltaPos;

    // Clic gauche
    int  leftClickCount;
    bool leftDown;
    bool leftReleased;

    // Clic droit
    int  rightClickCount;
    bool rightDown;
    bool rightReleased;

    // Roulette
    bool middleClick;
    int  wheel;
};
