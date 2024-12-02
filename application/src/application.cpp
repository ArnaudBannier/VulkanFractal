/*
    Copyright (c) Arnaud BANNIER and Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "application.hpp"
#include "model.hpp"
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

Application::Application(Framework &framework)
    : m_framework{ framework }
{
    m_maxIterationCount = 40;

    m_zMin = { -2.0, -1.0 };
    m_zMax = { +1.0, +1.0 };
    m_c = { 0.0, 0.0 };
    for (int i = 0; i < 10; i++)
        m_cFineTunings[i] = { 0.0, 0.0 };
    m_cLevel = 0;

    m_viewCenter = { 0.0, 0.0 };
    m_viewWidthZoom = 1.0;
    m_viewWidth = m_viewWidthZoom;

    // Colors
    m_colorExponent = 0.5f;
    m_colorCycles = 3.5f;

    m_paletteBase = { 0.5f, 0.5f, 0.5f };
    m_paletteAmplitude = { 0.5f, 0.5f, 0.5f };
    m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
    m_palettePhase = { 0.2f, 0.05f, 0.0f };
    m_paletteGlobalPhase = 0.f;

    m_fractalID = 0;

    m_animateZoom = false;
    m_animateColor = false;
    m_animateC = false;

    m_zoomSpeed = 1.f;
    m_zoomAccu = 0.f;

    m_cScale = 1.f;
    m_cAccuX = 0.f;
    m_cAccuY = 0.f;
    m_cSpeedX = 0.5f;
    m_cSpeedY = 0.5f;

    m_time = std::make_unique<Timer>();
}

void Application::run()
{
    vk::PhysicalDeviceProperties physicalProperties = m_framework.getProperties();
    vk::PhysicalDeviceMemoryProperties memoryProperties = m_framework.getMemoryProperties();
    vk::Device device = m_framework.getDevice();
    vk::PipelineCache pipelineCache = m_framework.getPipelineCache();
    vk::DescriptorPool descriptorPool = m_framework.getDescriptorPool();

    Renderer &renderer = m_framework.getRenderer();
    vk::RenderPass renderPass = renderer.getRenderPass();

    Window &m_window = m_framework.getWindow();

    VkDeviceSize minUniformBufferOffsetAlignment =
        physicalProperties.limits.minUniformBufferOffsetAlignment;

    // Allocate Uniform buffer
    std::vector<std::unique_ptr<Buffer>> uniformBufferObjects(Renderer::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uniformBufferObjects.size(); i++)
    {
        uniformBufferObjects[i] = std::make_unique<Buffer>(
            device,
            memoryProperties,
            1,
            sizeof(FractalUniform),
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );
        uniformBufferObjects[i]->map();
    }

    //==========================================================================
    // Fractal pipeline

    vk::DescriptorSetLayout fractalSetLayout =
        DescriptorSetLayoutBuilder()
        .addBinding(
            0, vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eVertex |
            vk::ShaderStageFlagBits::eFragment
        )
        .build(device);

    vk::PipelineLayout fractalPipelineLayout =
        PipelineLayoutBuilder()
        .addDescriptorSetLayout(fractalSetLayout)
        .build(device);

    vk::PipelineShaderStageCreateInfo fractalVertStage = tools::loadShader(
        device, "../shaders/fractal_shader.vert.spv",
        vk::ShaderStageFlagBits::eVertex);
    vk::PipelineShaderStageCreateInfo fractalFragStage = tools::loadShader(
        device, "../shaders/fractal_shader.frag.spv",
        vk::ShaderStageFlagBits::eFragment);

    vk::Pipeline fractalPipeline = PipelineBuilder(fractalPipelineLayout, renderPass, pipelineCache)
        .addVertexBindingDescription(0, sizeof(FullscreenModel::Vertex), vk::VertexInputRate::eVertex)
        .addVertexAttributeDescription(
            0, 0, vk::Format::eR32G32B32Sfloat,
            offsetof(FullscreenModel::Vertex, pos)
        )
        .addVertexAttributeDescription(
            1, 0, vk::Format::eR32G32Sfloat,
            offsetof(FullscreenModel::Vertex, texCoord)
        )
        .addShaderStage(fractalVertStage)
        .addShaderStage(fractalFragStage)
        .build(device);

    device.destroyShaderModule(fractalVertStage.module);
    device.destroyShaderModule(fractalFragStage.module);

    // Model
    FullscreenModel m_model(m_framework.getVulkanBase());

    // Descriptor sets
    std::vector<vk::DescriptorSet> fractalDescriptorSets =
        DescriptorSetBuilder()
        .addLayout(fractalSetLayout)
        .addLayout(fractalSetLayout)
        .build(device, descriptorPool);

    for (int i = 0; i < fractalDescriptorSets.size(); i++)
    {
        vk::DescriptorBufferInfo bufferInfo = uniformBufferObjects[i]->getDescriptorInfo();
        DescriptorSetUpdater()
            .beginDescriptorSet(fractalDescriptorSets[i])
            .addBuffer(
                0, vk::DescriptorType::eUniformBuffer, &bufferInfo)
            .update(device);
    }

    //==========================================================================
    // Setup ImGui

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;

    io.Fonts->AddFontFromFileTTF("../../assets/font/Roboto-Medium.ttf", 16.0f);

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForVulkan(m_framework.getWindow().getSDL());
    ImGui_ImplVulkan_InitInfo guiInitInfo = {};
    guiInitInfo.Instance = m_framework.getInstance();
    guiInitInfo.PhysicalDevice = m_framework.getPhysicalDevice();
    guiInitInfo.Device = device;
    guiInitInfo.QueueFamily = m_framework.getGraphicsQueueFamilyIndex();
    guiInitInfo.Queue = m_framework.getGraphicsQueue();
    guiInitInfo.PipelineCache = m_framework.getPipelineCache();
    guiInitInfo.DescriptorPool = m_framework.getDescriptorPool();
    guiInitInfo.RenderPass = m_framework.getRenderer().getRenderPass();
    guiInitInfo.Subpass = 0;
    guiInitInfo.MinImageCount = Renderer::MAX_FRAMES_IN_FLIGHT;
    guiInitInfo.ImageCount = Renderer::MAX_FRAMES_IN_FLIGHT;
    guiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    guiInitInfo.Allocator = nullptr;
    guiInitInfo.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&guiInitInfo);

    //==========================================================================
    // Boucle de rendu

    float accu = 0.f;
    m_time->start();

    // Input manager
    m_inputManager = std::make_unique<InputManager>();
    MouseInput *mouseInput = new MouseInput();

    m_inputManager->addInputGroup(MOUSE_INPUT, new MouseInput());
    m_inputManager->addInputGroup(APP_INPUT, new ApplicationInput());
    m_inputManager->addInputGroup(IMGUI_INPUT, new ImGuiInput());

    ApplicationInput *appInput = dynamic_cast<ApplicationInput *>(
        m_inputManager->getInputGroup(APP_INPUT));

    resetZoom();

    while (true)
    {
        m_time->update();

        // Process events
        m_inputManager->processEvents();
        if (appInput->hideGuiPressed) m_showUI = !m_showUI;
        if (appInput->quitPressed) break;

        // Update fractal
        updateFractal();

        // Check swapchain
        m_window.update();
        VkExtent2D extent = m_window.getExtent();
        if (extent.width < 8 || extent.height < 8 || m_window.isMinimized())
        {
            continue;
        }
        else if (m_window.isResized())
        {
            m_window.resetIsResized();
            renderer.recreateSwapchain(extent);
        }

        FractalUniform ubo{};
        ubo.z0 = { m_zMin.x, m_zMax.y };
        ubo.z1 = { m_zMax.x, m_zMin.y };
        ubo.juliaSet = (m_fractalID == 1);
        ubo.c.x = (double)m_c.x;
        ubo.c.y = (double)m_c.y;

        ubo.maxIterationCount = m_maxIterationCount;
        ubo.colorExponent = m_colorExponent;
        ubo.colorCycles = m_colorCycles * m_colorCycles;


        ubo.paletteBase = m_paletteBase;
        ubo.paletteAmplitude = m_paletteAmplitude;
        ubo.paletteFrequency.x = (float)m_paletteFrequency.x;
        ubo.paletteFrequency.y = (float)m_paletteFrequency.y;
        ubo.paletteFrequency.z = (float)m_paletteFrequency.z;
        ubo.palettePhase = m_palettePhase + m_paletteGlobalPhase;
        if (m_animateColor)
        {
            ubo.palettePhase += 0.2f * cosf(0.5f * m_time->getElapsed());
        }

        // Record command buffer
        vk::CommandBuffer commandBuffer = renderer.beginFrame();
        if (commandBuffer == nullptr) continue;
        uint32_t frameIndex = renderer.getFrameIndex();

        // Render pass
        renderer.beginRenderPass();

        uniformBufferObjects[frameIndex]->writeToBuffer(&ubo, sizeof(FractalUniform));

        // Fractal
        std::array<vk::DescriptorSet, 1> sets = {
            fractalDescriptorSets[frameIndex]
        };
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, fractalPipelineLayout,
            0, sets, nullptr);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, fractalPipeline);

        m_model.bind(commandBuffer);
        m_model.draw(commandBuffer);

        // UI
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        updateUIFrame();
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        renderer.endRenderPass();
        renderer.endFrame();
    }
    device.waitIdle();

    //==========================================================================
    // Free memory

    ImGui_ImplVulkan_Shutdown();

    device.freeDescriptorSets(descriptorPool, fractalDescriptorSets);

    device.destroyPipeline(fractalPipeline);
    device.destroyPipelineLayout(fractalPipelineLayout);
    device.destroyDescriptorSetLayout(fractalSetLayout);
}

void Application::updateFractal()
{
    float elapsed = m_time->getElapsed();
    float delta = m_time->getDelta();
    moveCamera();

    if (m_animateZoom)
    {
        m_zoomAccu += delta * powf(2.f, -m_zoomSpeed);
        double t = fmod(m_zoomAccu, 2.f);
        t = (t > 1.0) ? 2.0 - t : t; // Alternate
        t = t * t * (3.0 - 2.0 * t);     // Smoothstep

        double exponent = (1.0 - t) * log2(m_viewWidthZoom) + t * 2.0;
        m_viewWidth = pow(2.0, exponent);
    }
    else
    {
        m_viewWidth = m_viewWidthZoom;
    }
    m_c = glm::dvec2(0.0, 0.0);
    for (int i = 0; i < 10; i++)
        m_c += m_cFineTunings[i] * powf(4.f, -(float)i);

    if (m_animateC)
    {
        m_cAccuX += m_cSpeedX * delta;
        m_cAccuX = fmodf(m_cAccuX, 2.f * (float)M_PI);
        m_cAccuY += m_cSpeedY * delta;
        m_cAccuY = fmodf(m_cAccuY, 2.f * (float)M_PI);

        float scale = (float)(powf(2.f, -m_cScale) * m_viewWidth);
        m_c.x += cosf(m_cAccuX) * scale;
        m_c.y += sinf(m_cAccuY) * scale;
    }
    float aspectRatio = m_framework.getRenderer().getAspectRatio();
    m_zMin = m_viewCenter - 0.5 * m_viewWidth * glm::dvec2(1.0, 1.0 / aspectRatio);
    m_zMax = m_viewCenter + 0.5 * m_viewWidth * glm::dvec2(1.0, 1.0 / aspectRatio);
}

void Application::moveCamera()
{
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    MouseInput *mouseInput = dynamic_cast<MouseInput *>(
        m_inputManager->getInputGroup(MOUSE_INPUT));

    VkExtent2D windowExtent = m_framework.getWindow().getExtent();

    if (mouseInput->leftDown || mouseInput->rightDown)
    {
        double coeff = m_viewWidth / windowExtent.width;
        double deltaX = coeff * mouseInput->deltaPos.x;
        double deltaY = coeff * mouseInput->deltaPos.y;

        glm::dvec2 shift = glm::dvec2(-deltaX, deltaY);

        m_viewCenter += shift;
        m_zMin += shift;
        m_zMax += shift;
    }

    if (mouseInput->wheel != 0)
    {
        glm::dvec2 mouseRel;
        mouseRel.x = mouseInput->viewPos.x / (double)windowExtent.width;
        mouseRel.y = 1.0 - mouseInput->viewPos.y / (double)windowExtent.height;

        glm::dvec2 mousePos = m_zMin + mouseRel * (m_zMax - m_zMin);
        double coeff = (mouseInput->wheel > 0) ? 0.9 : (1.0 / 0.9);

        m_zMin = m_zMin + (1.0 - coeff) * (mousePos - m_zMin);
        m_zMax = m_zMax + (1.0 - coeff) * (mousePos - m_zMax);

        m_viewWidthZoom *= coeff;
        m_viewCenter = 0.5 * (m_zMin + m_zMax);
    }
}

void Application::updateUIFrame()
{
    ImGuiIO &io = ImGui::GetIO();

    if (m_showUI == false) return;

    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Fractal"))
    {
        if (ImGui::MenuItem("Mandelbrot", NULL, m_fractalID == 0))
        { 
            m_fractalID = 0;
            resetZoom();
        }
        if (ImGui::MenuItem("Julia", NULL, m_fractalID == 1))
        {
            m_fractalID = 1;
            resetZoom();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Information", NULL, &m_showInfo);
        ImGui::MenuItem("Configuration", NULL, &m_showConfig);
        ImGui::MenuItem("Animation", NULL, &m_showAnimation);
        ImGui::MenuItem("Palette selection", NULL, &m_showPalette);

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (m_showInfo)
    {
        ImGui::Begin("Informations", &m_showInfo, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("FPS = %d", (int)roundf(io.Framerate));
        ImGui::Text("Iteration count = %d", m_maxIterationCount);

        if (m_fractalID == 1)
        {
            ImGui::SeparatorText("Julia");
            const std::string cValue = "c = " + std::to_string(m_c.x) + " + " + std::to_string(m_c.y) + "i";
            ImGui::Text(cValue.c_str());
        }

        ImGui::End();
    }

    if (m_showConfig)
    {
        ImGui::Begin("Configuration", &m_showConfig, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SeparatorText("Main");

        ImGui::SliderInt("Max iter count", &m_maxIterationCount, 10, 1000);
        ImGui::SliderFloat("Color exponent", &m_colorExponent, 0.2f, 1.0f);
        ImGui::SliderFloat("Color cycles", &m_colorCycles, 1.0f, 4.0f);
        if (ImGui::Button("Reset zoom"))
        {
            resetZoom();
        }

        if (m_fractalID == 1)
        {
            ImGui::SeparatorText("Julia");

            bool pressed = ImGui::Button("Reset level");
            if (pressed)
            {
                for (int i = m_cLevel; i < 10; i++)
                {
                    m_cFineTunings[i] = glm::vec2(0.f, 0.f);
                }
            }

            ImGui::SliderInt("Fine tuning level", &m_cLevel, 0, 9);


            ImGui::SliderFloat("Real part", &(m_cFineTunings[m_cLevel].x), -1.f, 1.f, "%.2f");
            ImGui::SliderFloat("Imag part", &(m_cFineTunings[m_cLevel].y), -1.f, 1.f, "%.2f");
        }

        ImGui::End();
    }

    if (m_showAnimation)
    {
        ImGui::Begin("Animation", &m_showAnimation, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::Checkbox("Animate zoom", &m_animateZoom))
        {
            m_zoomAccu = 0.f;
        }
        ImGui::SliderFloat("Zoom speed", &m_zoomSpeed, 1.f, 10.f);
        ImGui::Checkbox("Animate color", &m_animateColor);

        if (m_fractalID == 1)
        {
            ImGui::SeparatorText("Julia");
            if (ImGui::Checkbox("Animate c", &m_animateC))
            {
                m_cAccuX = 0.f;
                m_cAccuY = 0.f;
            }
            ImGui::SliderFloat("c scale", &m_cScale, 1.f, 10.f);
            ImGui::SliderFloat("c speed X", &m_cSpeedX, 0.f, 1.f);
            ImGui::SliderFloat("c speed Y", &m_cSpeedY, 0.f, 1.f);
        }
        ImGui::End();
    }

    if (m_showPalette)
    {
        ImGui::Begin("Palette config", &m_showPalette, ImGuiWindowFlags_AlwaysAutoResize);

        bool pressed = false;

        ImGui::SeparatorText("Predefined");

        pressed = ImGui::Button("Default");
        if (pressed)
        {
            m_paletteBase = { 0.5f, 0.5f, 0.5f };
            m_paletteAmplitude = { 0.5f, 0.5f, 0.5f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.0f, 0.0f, 0.0f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P1");
        if (pressed)
        {
            m_paletteBase = { 0.5f, 0.5f, 0.5f };
            m_paletteAmplitude = { 0.5f, 0.5f, 0.3f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.8f, 0.0f, 0.0f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P2");
        if (pressed)
        {
            m_paletteBase = { 0.5f, 0.5f, 0.5f };
            m_paletteAmplitude = { 0.5f, 0.5f, 0.25f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.0f, 0.21f, 0.28f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P3");
        if (pressed)
        {
            m_paletteBase = { 0.5f, 0.5f, 0.5f };
            m_paletteAmplitude = { 0.5f, 0.5f, 0.3f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.0f, 0.15f, 0.3f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P4");
        if (pressed)
        {
            m_paletteBase = { 0.5f, 0.5f, 0.5f };
            m_paletteAmplitude = { 0.5f, 0.5f, 0.5f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.2f, 0.05f, 0.0f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P5");
        if (pressed)
        {
            m_paletteBase = { 0.0f, 0.5f, 0.65f };
            m_paletteAmplitude = { 0.7f, 0.5f, 0.35f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.0f, 0.07f, 0.0f };
        }

        ImGui::SameLine();
        pressed = ImGui::Button("P6");
        if (pressed)
        {
            m_paletteBase = { 0.45f, 0.45f, 0.4f };
            m_paletteAmplitude = { 0.4f, 0.4f, 0.1f };
            m_paletteFrequency = { 1.0f, 1.0f, 1.0f };
            m_palettePhase = { 0.8f, 0.0f, 0.07f };
        }

        ImGui::SeparatorText("Manual");

        ImGui::SliderFloat3("Base", (float *)&(m_paletteBase), 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat3("Amplitude", (float *)&(m_paletteAmplitude), 0.0f, 1.0f, "%.2f");
        ImGui::SliderInt3("Frequency", (int *)&(m_paletteFrequency), 0, 4);
        ImGui::SliderFloat3("Phase", (float *)&(m_palettePhase), 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Global phase", &m_paletteGlobalPhase, 0.0f, 1.0f, "%.2f");

        ImGui::End();
    }
}

void Application::resetZoom()
{
    if (m_fractalID == 0)
    {
        m_viewCenter = { -0.7f, 0.f };
    }
    else
    {
        m_viewCenter = { 0.f, 0.f };
    }
    m_viewWidthZoom = 5.0f;

    float aspectRatio = m_framework.getRenderer().getAspectRatio();
    m_zMin = m_viewCenter - 0.5 * m_viewWidth * glm::dvec2(1.0, 1.0 / aspectRatio);
    m_zMax = m_viewCenter + 0.5 * m_viewWidth * glm::dvec2(1.0, 1.0 / aspectRatio);
}
