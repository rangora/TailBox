#include "ShaderCompiler.h"
#include "Shader.h"
#include "spdlog/spdlog.h"
#include "CommonStatics.h"

namespace tb
{
    struct D3DShaderCompileInfo
    {
        std::string _entryPoint;
        std::string _target;
    };

    D3DShaderCompileInfo GetShaderInfo(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return {._entryPoint = "main", ._target = "vs_5_0"};
            case ShaderType::Pixel:
                return {._entryPoint = "main", ._target = "ps_5_0"};
            case ShaderType::None:
            default:
                return {._entryPoint = "None", ._target = "None"};
        }
    }

    ShaderCompiler::ShaderCompiler() = default;
    ShaderCompiler::~ShaderCompiler() = default;

    Shader* ShaderCompiler::CompileShader(const ShaderCompileDesc& desc)
    {
        uint32 compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        std::wstring wp = StringToWString(desc._path);

        ComPtr<ID3DBlob> bytecode = nullptr;
        ComPtr<ID3DBlob> err = nullptr;
        auto compileInfo = GetShaderInfo(desc._shaderType);

        if (FAILED(::D3DCompileFromFile(wp.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                        compileInfo._entryPoint.c_str(), compileInfo._target.c_str(), compileFlag, 0,
                                        &bytecode, &err)))
        {
            const char* errMsg = reinterpret_cast<const char*>(err->GetBufferPointer());
            spdlog::error("Shader Compile failed.. {}", errMsg);
            err->Release();
            err = nullptr;
            return nullptr;
        }

        Shader* newShader = new Shader;
        newShader->_identifier = desc._identifier;
        newShader->_bytecode = bytecode;

        return newShader;
    }
}