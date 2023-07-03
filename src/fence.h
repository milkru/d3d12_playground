#pragma once

struct Fence
{
    ID3D12Fence* pFence = nullptr;
    HANDLE eventHandle = NULL;
    UINT64 value = 0ull;
};

Fence createFence(_In_ Device& _rDevice, _In_opt_ UINT64 _initialValue = 0ull);

void flushCommandQueue(_In_ Device& _rDevice, _Inout_ Fence& _rFence);

void releaseFence(_Inout_ Fence& _rFence);
