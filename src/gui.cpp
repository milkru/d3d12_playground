#include "device.h"
#include "gui.h"

ContextGUI initializeGUI(_In_ xwin::Window& _rWindow, _In_ Device& _rDevice)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = 1u,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0u};

    ID3D12DescriptorHeap* pHeap = nullptr;
    DX_CALL(_rDevice.pDevice->CreateDescriptorHeap(&heapDesc,
                                                   IID_PPV_ARGS(&pHeap)));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.19f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.24f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.84f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 0.86f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.86f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.92f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.74f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;

    ImGui_ImplWin32_Init(_rWindow.getHwnd());
    ImGui_ImplDX12_Init(_rDevice.pDevice,
                        /*num_frames_in_flight*/ 1u,
                        DXGI_FORMAT_R8G8B8A8_UNORM,
                        pHeap,
                        pHeap->GetCPUDescriptorHandleForHeapStart(),
                        pHeap->GetGPUDescriptorHandleForHeapStart());

    return {.pHeap = pHeap,
            .pTextFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
                "../3rdparty/imgui/misc/fonts/DroidSans.ttf", 20.0f),
            .pTitleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
                "../3rdparty/imgui/misc/fonts/Roboto-Medium.ttf", 22.0f)};
}

void newFrameGUI(_In_ ContextGUI& _rContext, _In_ SettingsGUI& _rSettings)
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool bWindowHovered = false;
    static float windowAlpha = 0.68f;

    windowAlpha = glm::mix(windowAlpha, bWindowHovered ? 0.68f : 0.55f, 0.2f);
    ImGui::SetNextWindowBgAlpha(windowAlpha);

    ImGui::SetNextWindowPos(ImVec2(15, 20), ImGuiCond_FirstUseEver);

    ImGui::Begin("Menu",
                 0,
                 ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    bWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_None);

    {
        ImGui::PushFont(_rContext.pTitleFont);
        ImGui::SeparatorText("Stats");
        ImGui::PopFont();

        ImGui::PushFont(_rContext.pTextFont);
        ImGui::Text("%ls - Device", _rSettings.deviceName);
        ImGui::Text("%.3f ms - Frame time", _rSettings.frameTime);
        ImGui::Text("%.1f fps - Frame rate", _rSettings.frameRate);
        ImGui::PopFont();
    }

    ImGui::PushItemWidth(250.0f);
    {
        ImGui::PushFont(_rContext.pTitleFont);
        ImGui::SeparatorText("Lighting");
        ImGui::PopFont();

        ImGui::PushFont(_rContext.pTextFont);
        ImGui::ColorEdit3("Background color##3",
                          (float*)&_rSettings.backgroundColor,
                          ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3("Ambient color##3",
                          (float*)&_rSettings.ambientColor,
                          ImGuiColorEditFlags_NoInputs);
        ImGui::ColorEdit3("Sun color##3",
                          (float*)&_rSettings.sunColor,
                          ImGuiColorEditFlags_NoInputs);
        ImGui::DragFloat3("Sun direction",
                          (float*)&_rSettings.sunDirection,
                          0.01f,
                          -1.0f,
                          1.0f);
        ImGui::SliderFloat(
            "Sun intensity", &_rSettings.sunIntensity, 0.0f, 10.0f, "%.3f");
        ImGui::PopFont();
    }

    {
        ImGui::PushFont(_rContext.pTitleFont);
        ImGui::SeparatorText("Material");
        ImGui::PopFont();

        ImGui::PushFont(_rContext.pTextFont);
        ImGui::SliderFloat(
            "Smoothness", &_rSettings.smoothness, 0.0f, 1.0f, "%.3f");
        ImGui::PushItemWidth(250.0f);
        ImGui::Separator();
        ImGui::ColorPicker3("Albedo",
                            (float*)&_rSettings.albedoColor,
                            ImGuiColorEditFlags_NoAlpha |
                                ImGuiColorEditFlags_NoSidePreview |
                                ImGuiColorEditFlags_NoInputs);
        ImGui::Separator();
        ImGui::ColorPicker3("Specular",
                            (float*)&_rSettings.specularColor,
                            ImGuiColorEditFlags_NoAlpha |
                                ImGuiColorEditFlags_NoSidePreview |
                                ImGuiColorEditFlags_NoInputs);
        ImGui::PopItemWidth();
        ImGui::PopFont();
    }
    ImGui::PopItemWidth();

    /*
    {
        ImGui::PushFont(_rContext.pTitleFont);
        ImGui::SeparatorText("Post-processing");
        ImGui::PopFont();

        ImGui::PushFont(_rContext.pTextFont);
        ImGui::SliderFloat("Hue", &_rSettings.hue, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat(
            "Saturation", &_rSettings.saturation, 0.0f, 1.0f, "%.3f");
        ImGui::SliderFloat(
            "Brightness", &_rSettings.brightness, 0.0f, 1.0f, "%.3f");
        ImGui::PopFont();
    }
    */

    ImGui::End();
    ImGui::Render();
}

void drawFrameGUI(_In_ CommandList* _pCommandList, _In_ ContextGUI& _rContext)
{
    _pCommandList->SetDescriptorHeaps(1, &_rContext.pHeap);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _pCommandList);
}

void terminateGUI(_Inout_ ContextGUI& _rContext)
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    DX_SAFE_RELEASE(_rContext.pHeap);

    _rContext = {};
}
