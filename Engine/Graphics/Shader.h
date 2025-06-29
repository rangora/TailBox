#pragma once

#include "Core.h"
#include "RenderEnum.h"

namespace tb
{
    class Shader
    {
    public:
        Shader();
        ~Shader();

        ComPtr<ID3DBlob> _bytecode = nullptr;
        std::string _identifier;
    };
};