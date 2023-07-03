#include "device.h"
#include "fence.h"

Fence createFence(_In_ Device& _rDevice, _In_opt_ UINT64 _initialValue)
{
    ID3D12Fence* pFence = nullptr;
    DX_CALL(_rDevice.pDevice->CreateFence(
        _initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence)));

    HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    return {
        .pFence = pFence, .eventHandle = eventHandle, .value = _initialValue};
}

void flushCommandQueue(_In_ Device& _rDevice, _Inout_ Fence& _rFence)
{
    UINT64 signalValue = ++_rFence.value;
    DX_CALL(_rDevice.pCommandQueue->Signal(_rFence.pFence, signalValue));

    if (_rFence.pFence->GetCompletedValue() < signalValue)
    {
        DX_CALL(_rFence.pFence->SetEventOnCompletion(signalValue,
                                                     _rFence.eventHandle));
        WaitForSingleObject(_rFence.eventHandle, INFINITE);
    }
}

void releaseFence(_Inout_ Fence& _rFence)
{
    CloseHandle(_rFence.eventHandle);
    DX_SAFE_RELEASE(_rFence.pFence);

    _rFence = {};
}
