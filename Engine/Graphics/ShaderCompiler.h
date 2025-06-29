#pragma once

#include "Core.h"
#include "RenderEnum.h"

namespace tb
{
    class Shader;

    struct ShaderCompileDesc
    {
        ShaderType _shaderType = ShaderType::None;
        std::string _path;
        std::string _identifier;
    };

    class ShaderCompiler
    {
    public:
        static Shader* CompileShader(const ShaderCompileDesc& desc);

    private:
        ShaderCompiler();
        ~ShaderCompiler();
    };
} // namespace tb