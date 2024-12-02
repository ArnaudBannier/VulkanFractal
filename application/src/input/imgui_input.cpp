/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "imgui_input.hpp"

ImGuiInput::ImGuiInput()
    : InputGroup()
{
}

void ImGuiInput::onPreEventProcess()
{
}

void ImGuiInput::onEventProcess(SDL_Event evt)
{
    ImGui_ImplSDL2_ProcessEvent(&evt);
}

void ImGuiInput::onPostEventProcess()
{
}

void ImGuiInput::reset()
{
}
