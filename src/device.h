#pragma once

struct Device
{
    IDXGIFactory4* pFactory = nullptr;
    IDXGIAdapter1* pAdapter = nullptr;
    ID3D12Device* pDevice = nullptr;
    ID3D12DebugDevice* pDebugDevice = nullptr;
    ID3D12CommandQueue* pCommandQueue = nullptr;
    ID3D12CommandAllocator* pCommandAllocator = nullptr;
};

typedef ID3D12GraphicsCommandList CommandList;

Device createDevice();

CommandList* createCommandList(_In_ Device& _rDevice);

void transitionResource(_In_ CommandList* _pCommandList,
                        _In_ ID3D12Resource* _pResource,
                        _In_ D3D12_RESOURCE_STATES _stateBefore,
                        _In_ D3D12_RESOURCE_STATES _stateAfter);

void releaseDevice(_Inout_ Device& _rDevice);

void releaseCommandList(_Inout_ CommandList*& _pCommandList);
