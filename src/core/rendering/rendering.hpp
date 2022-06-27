#pragma once
#include "dependencies/std_include.hpp"

namespace rendering
{
    HRESULT __stdcall present(IDXGISwapChain* thisptr, UINT syncInterval, UINT flags);
    void initialize();
}
