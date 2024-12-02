/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/ve_input_group.hpp"

InputGroup::InputGroup() :
    m_enabled(true)
{
}

InputGroup::~InputGroup()
{
}

void InputGroup::onPreEventProcess()
{
}

void InputGroup::onEventProcess(SDL_Event evt)
{
}

void InputGroup::onPostEventProcess()
{
}

void InputGroup::reset()
{
}
