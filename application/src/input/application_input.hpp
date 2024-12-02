/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve.hpp"

class ApplicationInput : public InputGroup
{
public:
    ApplicationInput();

    virtual void onPreEventProcess() override;
    virtual void onEventProcess(SDL_Event evt) override;
    virtual void reset() override;

    bool quitPressed;
    bool hideGuiPressed;
};
