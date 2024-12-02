/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "ve.hpp"

#include "input/application_input.hpp"
#include "input/mouse_input.hpp"
#include "input/imgui_input.hpp"

struct FractalUniform
{
    glm::dvec2 z0;
    glm::dvec2 z1;
    glm::dvec2 c;
    bool juliaSet;
    int maxIterationCount;

    float colorExponent;
    float colorCycles;

    alignas(16) glm::vec3 paletteBase;
    alignas(16) glm::vec3 paletteAmplitude;
    alignas(16) glm::vec3 paletteFrequency;
    alignas(16) glm::vec3 palettePhase;
};

class Application
{
public:
    Application(Framework &framework);

    void run();

private:

    void updateFractal();
    void moveCamera();
    void updateUIFrame();
    void resetZoom();

    Framework &m_framework;
    std::unique_ptr<Timer> m_time;
    std::unique_ptr<InputManager> m_inputManager;
    enum InputID
    {
        MOUSE_INPUT, APP_INPUT, IMGUI_INPUT
    };

    int32_t m_fractalID;

    glm::dvec2 m_zMin, m_zMax;
    glm::dvec2 m_viewCenter;
    glm::dvec2 m_c;
    glm::vec2 m_cFineTunings[10];
    int m_cLevel;
    int m_cAnimationLevel;
    double m_viewWidth;
    double m_viewWidthZoom;

    int m_maxIterationCount;
    float m_colorExponent;
    float m_colorCycles;

    glm::vec3 m_paletteBase;
    glm::vec3 m_paletteAmplitude;
    glm::ivec3 m_paletteFrequency;
    glm::vec3 m_palettePhase;
    float m_paletteGlobalPhase;

    bool m_animateZoom;
    bool m_animateColor;
    bool m_animateC;
    float m_zoomSpeed;
    float m_zoomAccu;
    float m_cScale;
    float m_cSpeedX;
    float m_cSpeedY;
    float m_cAccuX;
    float m_cAccuY;

    bool m_showUI = true;
    bool m_showPalette = false;
    bool m_showInfo = false;
    bool m_showAnimation = false;
    bool m_showConfig = false;
};
