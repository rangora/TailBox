#include "ShaderCompiler.h"
#include "Shader.h"
#include "spdlog/spdlog.h"

namespace tb
{
    std::string GetEntryPoint(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return "VS_Main";
            case ShaderType::Pixel:
                return "PS_Main";
            case ShaderType::None:
            default:
                return "None";
        }
    }

    std::string GetTarget(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return "vs_5_0";
            case ShaderType::Pixel:
                return "ps_5_0";
            case ShaderType::None:
            default:
                return "None";
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

        std::wstring wp(desc._path.size(), L'\0');
        std::mbstowcs(&wp[0], desc._path.c_str(), desc._path.size());


        ComPtr<ID3DBlob> bytecode = nullptr;
        ComPtr<ID3DBlob> err = nullptr;
        std::string entryPoint = GetEntryPoint(desc._shaderType);
        std::string target = GetTarget(desc._shaderType);

        if (FAILED(::D3DCompileFromFile(wp.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(),
                                        target.c_str(), compileFlag, 0, &bytecode, &err)))
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