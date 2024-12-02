/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"
#include "core/ve_input_group.hpp"

class InputManager
{
public:
    InputManager();
    InputManager(InputManager const&) = delete;
    InputManager& operator=(InputManager const&) = delete;
    ~InputManager();

    std::set<SDL_GameController *> getGameControllers();
    int getGameControllerCount() const;

    void processEvents();

    void setKeyboardIsPlayer(bool keyboardIsPlayer);
    bool keyboardIsPlayer() const;
    void resetPlayerMapping();

    void addInputGroup(int groupID, InputGroup *inputGroup);
    InputGroup *getInputGroup(int groupID);

    void setMaxPlayerCount(int maxPlayerCount);
    int getMaxPlayerCount() const;

protected:
    void addGameController(int joystickID);
    void removeGameController(int joystickID);
    void updatePlayerCount();

    std::map<int, InputGroup *> m_inputMap;

    int m_maxPlayerCount;
    int m_playerCount;
    bool m_keyboardIsPlayer;
    std::set<SDL_GameController *> m_gameControllers;
};

inline SDL_JoystickID getControllerInstanceID(SDL_GameController *controller)
{
    return SDL_JoystickInstanceID(
        SDL_GameControllerGetJoystick(controller));
}

inline void InputManager::setKeyboardIsPlayer(bool keyboardIsPlayer)
{
    m_keyboardIsPlayer = keyboardIsPlayer;
    resetPlayerMapping();
}

inline bool InputManager::keyboardIsPlayer() const
{
    return m_keyboardIsPlayer;
}

inline void InputManager::setMaxPlayerCount(int maxPlayerCount)
{
    m_maxPlayerCount = maxPlayerCount;
}

inline int InputManager::getMaxPlayerCount() const
{
    return m_maxPlayerCount;
}
