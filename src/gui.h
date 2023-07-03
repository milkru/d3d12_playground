#pragma once

struct ContextGUI
{
    ID3D12DescriptorHeap* pHeap = nullptr;
    ImFont* pTextFont = nullptr;
    ImFont* pTitleFont = nullptr;
};

struct SettingsGUI
{
    // Stats
    const wchar_t* deviceName = L"";
    float frameTime = 16.0f;
    float frameRate = 60.0f;

    // Lighting
    glm::vec3 backgroundColor{};
    glm::vec3 ambientColor{};
    glm::vec3 sunColor{};
    glm::vec3 cameraPosition{};
    glm::vec3 sunDirection{};
    float sunIntensity = 1.0f;

    // Material
    glm::vec3 albedoColor{};
    glm::vec3 specularColor{};
    float smoothness = 1.0f;

    // Post-processing
    float hue = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
};

ContextGUI initializeGUI(_In_ xwin::Window& _rWindow, _In_ Device& _rDevice);

void newFrameGUI(_In_ ContextGUI& _rContext, _In_ SettingsGUI& _rSettings);

void drawFrameGUI(_In_ CommandList* _pCommandList, _In_ ContextGUI& _rContext);

void terminateGUI(_Inout_ ContextGUI& _rContext);
