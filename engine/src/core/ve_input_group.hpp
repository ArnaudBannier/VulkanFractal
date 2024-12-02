/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"

class InputGroup
{
public:
    InputGroup();
    InputGroup(InputGroup const&) = delete;
    InputGroup& operator=(InputGroup const&) = delete;
    virtual ~InputGroup();

    virtual void onPreEventProcess();
    virtual void onEventProcess(SDL_Event evt);
    virtual void onPostEventProcess();
    virtual void reset();

    void setEnabled(bool enabled);
    bool isEnabled();

protected:
    bool m_enabled;
};

inline void InputGroup::setEnabled(bool enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;
        reset();
    }
}

inline bool InputGroup::isEnabled()
{
    return m_enabled;
}
