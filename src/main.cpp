#include "device.h"
#include "fence.h"
#include "shader.h"
#include "pipeline.h"
#include "swapchain.h"
#include "buffer.h"
#include "texture.h"
#include "mesh.h"
#include "gui.h"

#include <chrono>

const uint32_t kWindowWidth = 1280u;
const uint32_t kWindowHeight = 720u;

const uint32_t kSwapChainBufferCount = 2u;
const bool kbVsyncEnabled = true;

typedef std::chrono::time_point<std::chrono::steady_clock> TimeStamp;

void xmain(_In_ int32_t _argc, _In_ const char** _ppArgv)
{
    if (_argc - 1 == 0)
    {
        printf("Provide mesh path as command argument.\n");
        return;
    }

    const char* meshPath = _ppArgv[1];

    xwin::EventQueue eventQueue;
    xwin::Window window;
    assert(window.create({.width = kWindowWidth,
                          .height = kWindowHeight,
                          .visible = true,
                          .title = "Playground",
                          .name = "d3d12_playground"},
                         eventQueue));

    Device device = createDevice();

    xwin::UVec2 windowSize = window.getWindowSize();
    SwapChain swapChain =
        createSwapChain(window,
                        device,
                        {.width = windowSize.x,
                         .height = windowSize.y,
                         .bufferCount = kSwapChainBufferCount});

    uint32_t frameIndex = swapChain.pSwapChain->GetCurrentBackBufferIndex();
    Fence fence = createFence(device);

    Shader* vertexShader = createShader(
        {.path = L"../src/shaders/geometry.hlsl", .target = "vs_5_0"});

    Shader* pixelShader = createShader(
        {.path = L"../src/shaders/color.hlsl", .target = "ps_5_0"});

    DepthStencil depthStencil = createDepthStencil(device, swapChain.extent);

    Pipeline pipeline = createPipeline(device, vertexShader, pixelShader);

    releaseShader(vertexShader);
    releaseShader(pixelShader);

    Mesh mesh = createMesh(device, meshPath);

    struct FrameConstants
    {
        glm::mat4 projectionMatrix{};
        glm::mat4 modelMatrix{};
        glm::mat4 viewMatrix{};
        glm::vec3 ambientColor{};
        glm::vec3 sunColor{};
        glm::vec3 cameraPosition{};
        glm::vec3 sunDirection{};
        glm::vec3 albedoColor{};
        glm::vec3 specularColor{};
        float sunIntensity = 0.0f;
        float smoothness = 0.0f;
    } frameConstants;

    ConstantBuffer constantBuffer =
        createConstantBuffer<FrameConstants>(device);

    CommandList* pCommandList = createCommandList(device);

    DXGI_ADAPTER_DESC adapterDesc;
    device.pAdapter->GetDesc(&adapterDesc);

    ContextGUI guiContext = initializeGUI(window, device);
    SettingsGUI guiSettings = {.deviceName = adapterDesc.Description,
                               .backgroundColor = {0.23f, 0.25f, 0.26f},
                               .ambientColor = {0.46f, 0.58f, 0.67f},
                               .sunColor = {0.98f, 0.92f, 0.77f},
                               .cameraPosition = {1.0f, 0.5f, 1.0f},
                               .sunDirection = {-1.0f, 0.5f, 1.0f},
                               .sunIntensity = 1.2f,
                               .albedoColor = {1.0f, 0.7f, 0.7f},
                               .specularColor = {0.7f, 0.8f, 1.0f},
                               .smoothness = 0.35f};

    // Flush GPU memory initializations
    flushCommandQueue(device, fence);

    auto recordCommands = [&](CommandList* _pCommandList)
    {
        // Reuses the memory from allocated lists
        DX_CALL(device.pCommandAllocator->Reset());
        DX_CALL(_pCommandList->Reset(device.pCommandAllocator,
                                     pipeline.pPipelineState));

        _pCommandList->SetGraphicsRootSignature(pipeline.pRootSignature);
        _pCommandList->SetDescriptorHeaps(1u, &constantBuffer.pHeap);

        // Connect a descriptor to a root slot
        _pCommandList->SetGraphicsRootDescriptorTable( // Vertex shader
            /*RootParameterIndex*/ 0u,
            constantBuffer.pHeap->GetGPUDescriptorHandleForHeapStart());

        _pCommandList->SetGraphicsRootDescriptorTable( // Pixel shader
            /*RootParameterIndex*/ 1u,
            constantBuffer.pHeap->GetGPUDescriptorHandleForHeapStart());

        transitionResource(_pCommandList,
                           swapChain.renderTargets[frameIndex],
                           D3D12_RESOURCE_STATE_PRESENT,
                           D3D12_RESOURCE_STATE_RENDER_TARGET);

        {
            D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle =
                swapChain.pRenderTargetHeap
                    ->GetCPUDescriptorHandleForHeapStart();

            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle =
                depthStencil.pHeap->GetCPUDescriptorHandleForHeapStart();

            UINT renderTargetDescriptorSize =
                device.pDevice->GetDescriptorHandleIncrementSize(
                    D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            renderTargetHandle.ptr +=
                frameIndex * SIZE_T(renderTargetDescriptorSize);
            _pCommandList->OMSetRenderTargets(
                1u, &renderTargetHandle, FALSE, &depthStencilHandle);

            float clearColor[] = {guiSettings.backgroundColor.r,
                                  guiSettings.backgroundColor.g,
                                  guiSettings.backgroundColor.b,
                                  1.0f};

            _pCommandList->ClearRenderTargetView(
                renderTargetHandle, clearColor, 0u, nullptr);

            _pCommandList->ClearDepthStencilView(depthStencilHandle,
                                                 D3D12_CLEAR_FLAG_DEPTH,
                                                 1.0f,
                                                 0u,
                                                 0u,
                                                 nullptr);
        }

        D3D12_VIEWPORT viewport = {.TopLeftX = 0.0f,
                                   .TopLeftY = 0.0f,
                                   .Width = FLOAT(swapChain.extent.x),
                                   .Height = FLOAT(swapChain.extent.y),
                                   .MinDepth = 0.0f,
                                   .MaxDepth = 1.0f};

        _pCommandList->RSSetViewports(1u, &viewport);

        D3D12_RECT scissor = {.left = 0l,
                              .top = 0l,
                              .right = LONG(swapChain.extent.x),
                              .bottom = LONG(swapChain.extent.y)};

        _pCommandList->RSSetScissorRects(1u, &scissor);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {
            .BufferLocation = mesh.vertexBuffer.gpuAddress,
            .SizeInBytes = mesh.vertexBuffer.sizeInBytes,
            .StrideInBytes = mesh.vertexBuffer.strideInBytes};

        _pCommandList->IASetVertexBuffers(0u, 1u, &vertexBufferView);

        D3D12_INDEX_BUFFER_VIEW indexBufferView = {
            .BufferLocation = mesh.indexBuffer.gpuAddress,
            .SizeInBytes = mesh.indexBuffer.sizeInBytes,
            .Format = DXGI_FORMAT_R32_UINT};

        _pCommandList->IASetIndexBuffer(&indexBufferView);

        _pCommandList->IASetPrimitiveTopology(
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        _pCommandList->DrawIndexedInstanced(mesh.indexCount, 1u, 0u, 0u, 0u);

        drawFrameGUI(_pCommandList, guiContext);

        transitionResource(_pCommandList,
                           swapChain.renderTargets[frameIndex],
                           D3D12_RESOURCE_STATE_RENDER_TARGET,
                           D3D12_RESOURCE_STATE_PRESENT);

        _pCommandList->Close();
    };

    TimeStamp previousTime = std::chrono::high_resolution_clock::now();

    bool bShouldClose = false;
    while (!bShouldClose)
    {
        eventQueue.update();

        bool bShouldRender = true;
        while (!eventQueue.empty())
        {
            const xwin::Event& rEvent = eventQueue.front();

            {
                xwin::UVec2 windowSize = window.getWindowSize();

                ImGuiIO& rIO = ImGui::GetIO();
                rIO.DisplaySize =
                    ImVec2(float(windowSize.x), float(windowSize.y));
            }

            if (rEvent.type == xwin::EventType::MouseInput)
            {
                const xwin::MouseInputData& rMouseInput =
                    rEvent.data.mouseInput;

                ImGuiIO& rIO = ImGui::GetIO();

                rIO.MouseDown[0] =
                    rMouseInput.button == xwin::MouseInput::Left &&
                    rMouseInput.state == xwin::ButtonState::Pressed;
                rIO.MouseDown[1] =
                    rMouseInput.button == xwin::MouseInput::Right &&
                    rMouseInput.state == xwin::ButtonState::Pressed;
            }

            if (rEvent.type == xwin::EventType::Resize)
            {
                xwin::ResizeData data = rEvent.data.resize;

                if (data.width != 0u && data.height != 0u)
                {
                    flushCommandQueue(device, fence);
                    releaseDepthStencil(depthStencil);

                    resizeSwapChain(window,
                                    device,
                                    swapChain,
                                    {.width = data.width,
                                     .height = data.height,
                                     .bufferCount = kSwapChainBufferCount});

                    depthStencil = createDepthStencil(device, swapChain.extent);
                    frameIndex = swapChain.pSwapChain->GetCurrentBackBufferIndex();

                    bShouldRender = false;
                }
            }

            if (rEvent.type == xwin::EventType::Close)
            {
                window.close();
                bShouldRender = false;
                bShouldClose = true;
            }

            eventQueue.pop();
        }

        if (bShouldRender)
        {
            newFrameGUI(guiContext, guiSettings);

            {
                // Setup frame constants
                static TimeStamp startTime =
                    std::chrono::high_resolution_clock::now();

                static TimeStamp previousTime = startTime;

                TimeStamp currentTime =
                    std::chrono::high_resolution_clock::now();

                float deltaTime =
                    std::chrono::duration<float,
                                          std::chrono::milliseconds::period>(
                        currentTime - previousTime)
                        .count();

                guiSettings.frameTime = deltaTime;
                guiSettings.frameRate = 1000.0f / deltaTime;

                previousTime = currentTime;

                float time =
                    std::chrono::duration<float, std::chrono::seconds::period>(
                        currentTime - startTime)
                        .count();

                previousTime = currentTime;

                xwin::UVec2 windowSize = window.getWindowSize();

                const float fov = 45.0f;
                const float zoom = 1.5f;

                frameConstants = {
                    .projectionMatrix = glm::perspective(
                        glm::radians(fov),
                        float(windowSize.x) / float(windowSize.y),
                        0.01f,
                        100.0f),

                    .modelMatrix = glm::rotate(glm::mat4(1.0f),
                                               0.5f * time * glm::radians(-90.0f),
                                               glm::vec3(0.0f, 1.0f, 0.0f)),

                    .viewMatrix = glm::lookAt(guiSettings.cameraPosition,
                                              glm::vec3(0.3f, 0.05f, 0.0f),
                                              glm::vec3(0.0f, 1.0f, 0.0f)),

                    .ambientColor = guiSettings.ambientColor,
                    .sunColor = guiSettings.sunColor,
                    .sunDirection = guiSettings.sunDirection,
                    .albedoColor = guiSettings.albedoColor,
                    .specularColor = guiSettings.specularColor,
                    .sunIntensity = guiSettings.sunIntensity,
                    .smoothness = guiSettings.smoothness};

                uploadBufferData(constantBuffer, &frameConstants);
            }

            recordCommands(pCommandList);

            {
                // Finalize frame
                device.pCommandQueue->ExecuteCommandLists(
                    1u, (ID3D12CommandList**)&pCommandList);

                swapChain.pSwapChain->Present(kbVsyncEnabled ? 1u : 0u, 0u);

                flushCommandQueue(device, fence);
                frameIndex = swapChain.pSwapChain->GetCurrentBackBufferIndex();
            }
        }
    }

    {
        terminateGUI(guiContext);
        releaseCommandList(pCommandList);
        releaseConstantBuffer(constantBuffer);
        releaseMesh(mesh);
        releasePipeline(pipeline);
        releaseDepthStencil(depthStencil);
        releaseFence(fence);
        releaseSwapChain(swapChain);
        releaseDevice(device);
    }
}
