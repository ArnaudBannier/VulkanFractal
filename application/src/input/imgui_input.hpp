/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve.hpp"

class ImGuiInput : public InputGroup
{
public:
    ImGuiInput();

    virtual void onPreEventProcess() override;
    virtual void onEventProcess(SDL_Event evt) override;
    virtual void onPostEventProcess() override;
    virtual void reset() override;
};
