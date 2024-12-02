/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve_settings.hpp"

/// @ingroup Timer
/// @brief Structure représentant un chronomètre.
class Timer
{
public:
    Timer();
    Timer(Timer const&) = delete;
    Timer& operator=(Timer const&) = delete;

    void start();
    void update();
    void update(float deltaTime);
    void update(Uint64 deltaTimeMS);

    void setMaximumDeltaTime(float maxDelta);
    void setTimeScale(float scale);
    float getTimeScale() const;
    float getDelta() const;
    float getUnscaledDelta() const;
    float getElapsed() const;
    float getUnscaledElapsed() const;

    Uint64 getDeltaMS() const;
    Uint64 getUnscaledDeltaMS() const;
    Uint64 getElapsedMS() const;
    Uint64 getUnscaledElapsedMS() const;

protected:
    /// @protected
    /// @brief Temps de départ.
    Uint64 m_startTime;

    /// @protected
    /// @brief Temps du dernier appel à Timer_Update().
    Uint64 m_currentTime;

    /// @protected
    /// @brief Temps de l'avant dernier appel à Timer_Update().
    Uint64 m_previousTime;

    /// @protected
    /// @brief Ecart entre les deux derniers appels à Timer_Update().
    Uint64 m_delta;
    Uint64 m_unscaledDelta;

    float m_scale;

    Uint64 m_maxDelta;

    Uint64 m_elapsed;
    Uint64 m_unscaledElapsed;
};

inline void Timer::setMaximumDeltaTime(float maxDelta)
{
    m_maxDelta = static_cast<Uint64>(maxDelta * 1000.0);
}

inline void Timer::setTimeScale(float scale)
{
    m_scale = scale;
}

inline float Timer::getDelta() const
{
    return static_cast<float>(m_delta) / 1000.f;
}

inline float Timer::getTimeScale() const
{
    return m_scale;
};

inline float Timer::getUnscaledDelta() const
{
    return static_cast<float>(m_unscaledDelta) / 1000.f;
}

inline float Timer::getElapsed() const
{
    return static_cast<float>(m_elapsed) / 1000.f;
}

inline float Timer::getUnscaledElapsed() const
{
    return static_cast<float>(m_unscaledElapsed) / 1000.f;
}

inline Uint64 Timer::getDeltaMS() const
{
    return m_delta;
}

inline Uint64 Timer::getUnscaledDeltaMS() const
{
    return m_unscaledElapsed;
}

inline Uint64 Timer::getElapsedMS() const
{
    return m_elapsed;
}

inline Uint64 Timer::getUnscaledElapsedMS() const
{
    return m_unscaledElapsed;
}

inline void Timer::update(float deltaTime)
{
    update(static_cast<Uint64>(deltaTime * 1000.0));
}
