#include <CrossWindow/CrossWindow.h>
#include <CrossWindow/Graphics.h>

#define GLM_FORCE_SSE42 1
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES 1
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <direct.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

#ifndef DX_CALL
#define DX_CALL(_call)                                                         \
    do                                                                         \
    {                                                                          \
        HRESULT result_ = _call;                                               \
        assert(result_ == S_OK);                                               \
    } while (0)
#endif // DX_CALL

#ifndef DX_SAFE_RELEASE
#define DX_SAFE_RELEASE(_handle)                                               \
    do                                                                         \
    {                                                                          \
        if (_handle)                                                           \
        {                                                                      \
            _handle->Release();                                                \
            _handle = nullptr;                                                 \
        }                                                                      \
    } while (0)
#endif // DX_SAFE_RELEASE
