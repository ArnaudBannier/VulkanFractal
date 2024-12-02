/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/ve_input_manager.hpp"

InputManager::InputManager() :
    m_inputMap(), m_gameControllers(), m_keyboardIsPlayer(false),
    m_playerCount(0), m_maxPlayerCount(4)
{
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            SDL_GameController *controller = SDL_GameControllerOpen(i);
            m_gameControllers.insert(controller);
        }
    }
    resetPlayerMapping();
}

InputManager::~InputManager()
{
    for (auto pair : m_inputMap)
    {
        delete pair.second;
    }

    for (SDL_GameController *controller : m_gameControllers)
    {
        SDL_GameControllerClose(controller);
    }
}

void InputManager::processEvents()
{
    for (auto &pair : m_inputMap)
    {
        InputGroup *group = pair.second;
        if (group->isEnabled())
        {
            group->onPreEventProcess();
        }
    }

    SDL_Event evt;
    SDL_GameController *controller = nullptr;
    int playerID = 0;
    while (SDL_PollEvent(&evt))
    {
        bool skipEvent = false;

        switch (evt.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
            addGameController(evt.cdevice.which);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            removeGameController(evt.cdevice.which);
            break;

        case SDL_CONTROLLERAXISMOTION:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERDEVICEREMAPPED:
        case SDL_CONTROLLERTOUCHPADDOWN:
        case SDL_CONTROLLERTOUCHPADMOTION:
        case SDL_CONTROLLERTOUCHPADUP:
        case SDL_CONTROLLERSENSORUPDATE:
            controller = SDL_GameControllerFromInstanceID(evt.caxis.which);
            if (controller == nullptr)
            {
                skipEvent = true;
                break;
            }
            playerID = SDL_GameControllerGetPlayerIndex(controller);
            if (playerID < 0 || playerID >= m_maxPlayerCount)
            {
                skipEvent = true;
                break;
            }
            break;

        default:
            break;
        }

        if (skipEvent) continue;


        for (auto &pair : m_inputMap)
        {
            InputGroup *group = pair.second;
            if (group->isEnabled())
            {
                group->onEventProcess(evt);
            }
        }
    }

    for (auto &pair : m_inputMap)
    {
        InputGroup *group = pair.second;
        if (group->isEnabled())
        {
            group->onPostEventProcess();
        }
    }
}

void InputManager::addGameController(int joystickID)
{
    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);

    for (SDL_GameController *controller : m_gameControllers)
    {
        int id = SDL_GameControllerGetPlayerIndex(controller);
        if (id < 0 || id >= m_maxPlayerCount) continue;

        playerIDs[id] = true;

        if (getControllerInstanceID(controller) == joystickID)
        {
            // Le controller existe déjà
            return;
        }
    }

    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (int i = playerID; i < m_maxPlayerCount; i++)
    {
        if (playerIDs[i]) continue;

        playerID = i;
        break;
    }

    SDL_GameController *controller = SDL_GameControllerOpen(joystickID);
    SDL_GameControllerSetPlayerIndex(controller, playerID);
    m_gameControllers.insert(controller);

    updatePlayerCount();
}

void InputManager::removeGameController(int joystickID)
{
    SDL_GameController *controller = SDL_GameControllerFromInstanceID(joystickID);
    if (controller == nullptr) return;

    m_gameControllers.erase(controller);
    SDL_GameControllerClose(controller);

    updatePlayerCount();
}

void InputManager::updatePlayerCount()
{
    m_playerCount = 1;

    bool *playerIDs = new bool[m_maxPlayerCount];
    std::fill_n(playerIDs, m_maxPlayerCount, false);
    playerIDs[0] = true;

    for (SDL_GameController *controller : m_gameControllers)
    {
        int id = SDL_GameControllerGetPlayerIndex(controller);
        if (id < 0 || id >= m_maxPlayerCount) continue;

        m_playerCount += playerIDs[id] ? 0 : 1;
        playerIDs[id] = true;
    }
}

void InputManager::resetPlayerMapping()
{
    int playerID = m_keyboardIsPlayer ? 1 : 0;
    for (SDL_GameController *controller : m_gameControllers)
    {
        playerID = playerID < m_maxPlayerCount - 1 ? playerID : m_maxPlayerCount - 1;
        SDL_GameControllerSetPlayerIndex(controller, playerID++);
    }
    updatePlayerCount();
}

void InputManager::addInputGroup(int groupID, InputGroup *inputGroup)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end())
    {
        delete it->second;
        m_inputMap.erase(groupID);
    }

    m_inputMap.insert(std::make_pair(groupID, inputGroup));
}

InputGroup *InputManager::getInputGroup(int groupID)
{
    auto it = m_inputMap.find(groupID);
    if (it != m_inputMap.end()) return it->second;
    return nullptr;
}

std::set<SDL_GameController *> InputManager::getGameControllers()
{
    return m_gameControllers;
}

int InputManager::getGameControllerCount() const
{
    return (int)m_gameControllers.size();
}
