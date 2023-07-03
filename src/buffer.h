#pragma once

struct Buffer
{
    ID3D12Resource* pResource = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = 0ull;
    uint32_t sizeInBytes = 0u;
    uint32_t strideInBytes = 0u;
};

struct ConstantBuffer : public Buffer
{
    ID3D12DescriptorHeap* pHeap = nullptr;
};

Buffer createBuffer(_In_ Device& _rDevice,
                    _In_ uint32_t _sizeInBytes,
                    _In_ uint32_t _strideInBytes);

void uploadBufferData(_In_ Buffer& _rBuffer, _In_ void* _pData);

void releaseBuffer(_Inout_ Buffer& _rDevice);

void releaseConstantBuffer(_Inout_ ConstantBuffer& _rDevice);

#include "buffer.hpp"
