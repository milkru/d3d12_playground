#include "device.h"

static IDXGIFactory4* createFactory()
{
    UINT factoryFlags = 0u;
    ID3D12Debug1* pDebug = nullptr;

#ifdef DEBUG_
    ID3D12Debug* pDebugInterface = nullptr;
    DX_CALL(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugInterface)));

    DX_CALL(pDebugInterface->QueryInterface(IID_PPV_ARGS(&pDebug)));
    pDebug->EnableDebugLayer();
    pDebug->SetEnableGPUBasedValidation(true);

    factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

    pDebugInterface->Release();
    pDebugInterface = nullptr;
#endif // DEBUG_

    IDXGIFactory4* pFactory = nullptr;
    DX_CALL(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&pFactory)));

    return pFactory;
}

static IDXGIAdapter1* tryPickAdapter(_In_ IDXGIFactory4* _pFactory)
{
    IDXGIAdapter1* pAdapter = nullptr;

    for (UINT adapterIndex = 0u;
         _pFactory->EnumAdapters1(adapterIndex, &pAdapter) !=
         DXGI_ERROR_NOT_FOUND;
         ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 adapterDesc;
        pAdapter->GetDesc1(&adapterDesc);

        // Skip software adapters
        if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        // Find an adapter which supports DX12
        if (SUCCEEDED(D3D12CreateDevice(pAdapter,
                                        D3D_FEATURE_LEVEL_12_0,
                                        __uuidof(ID3D12Device),
                                        nullptr)))
        {
            break;
        }

        pAdapter->Release();
    }

    return pAdapter;
}

static ID3D12Device* createDevice(_In_ IDXGIAdapter1* _pAdapter)
{
    ID3D12Device* pDevice = nullptr;
    D3D12CreateDevice(
        _pAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));
    return pDevice;
}

static ID3D12DebugDevice* createDebugDevice(_In_ ID3D12Device* _pDevice)
{
    ID3D12DebugDevice* pDebugDevice = nullptr;

#ifdef DEBUG_
    DX_CALL(_pDevice->QueryInterface(&pDebugDevice));
#endif // DEBUG_

    return pDebugDevice;
}

static ID3D12CommandQueue* createCommandQueue(_In_ ID3D12Device* _pDevice)
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = 0,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0u};

    ID3D12CommandQueue* pCommandQueue = nullptr;
    DX_CALL(_pDevice->CreateCommandQueue(&commandQueueDesc,
                                         IID_PPV_ARGS(&pCommandQueue)));

    return pCommandQueue;
}

static ID3D12CommandAllocator*
    createCommandAllocator(_In_ ID3D12Device* _pDevice)
{
    ID3D12CommandAllocator* pCommandAllocator = nullptr;
    DX_CALL(_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                             IID_PPV_ARGS(&pCommandAllocator)));
    return pCommandAllocator;
}

Device createDevice()
{
    IDXGIFactory4* pFactory = createFactory();
    IDXGIAdapter1* pAdapter = tryPickAdapter(pFactory);
    assert(pAdapter != nullptr);

    ID3D12Device* pDevice = createDevice(pAdapter);
    ID3D12DebugDevice* pDebugDevice = createDebugDevice(pDevice);
    ID3D12CommandQueue* pCommandQueue = createCommandQueue(pDevice);
    ID3D12CommandAllocator* pCommandAllocator = createCommandAllocator(pDevice);

    return {.pFactory = pFactory,
            .pAdapter = pAdapter,
            .pDevice = pDevice,
            .pDebugDevice = pDebugDevice,
            .pCommandQueue = pCommandQueue,
            .pCommandAllocator = pCommandAllocator};
}

CommandList* createCommandList(_In_ Device& _rDevice)
{
    ID3D12GraphicsCommandList* pCommandList = nullptr;
    DX_CALL(_rDevice.pDevice->CreateCommandList(0u,
                                                D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                _rDevice.pCommandAllocator,
                                                nullptr,
                                                IID_PPV_ARGS(&pCommandList)));

    // Command lists are by default set to recording state after creation
    DX_CALL(pCommandList->Close());
    return (CommandList*)pCommandList;
}

void transitionResource(_In_ CommandList* _pCommandList,
                        _In_ ID3D12Resource* _pResource,
                        _In_ D3D12_RESOURCE_STATES _stateBefore,
                        _In_ D3D12_RESOURCE_STATES _stateAfter)
{
    D3D12_RESOURCE_BARRIER barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {.pResource = _pResource,
                       .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                       .StateBefore = _stateBefore,
                       .StateAfter = _stateAfter}};

    _pCommandList->ResourceBarrier(1u, &barrier);
}

void releaseDevice(_Inout_ Device& _rDevice)
{
    DX_SAFE_RELEASE(_rDevice.pCommandAllocator);
    DX_SAFE_RELEASE(_rDevice.pCommandQueue);
    DX_SAFE_RELEASE(_rDevice.pDevice);
    DX_SAFE_RELEASE(_rDevice.pAdapter);
    DX_SAFE_RELEASE(_rDevice.pFactory);

#if DEBUG_
    _rDevice.pDebugDevice->ReportLiveDeviceObjects(
        D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);

    DX_SAFE_RELEASE(_rDevice.pDebugDevice);
#endif // DEBUG_

    _rDevice = {};
}

void releaseCommandList(_Inout_ CommandList*& _pCommandList)
{
    DX_SAFE_RELEASE(_pCommandList);
}
