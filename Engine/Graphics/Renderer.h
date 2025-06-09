#pragma once

#include "Core.h"
#include "GpuBuffer.h"
#include "Shader.h"
#include "DX12Device.h"

namespace tb
{
    class Renderer
    {
    public:
        static Renderer* Get()
        {
            static Renderer instance;
            return &instance;
        }

        void Initialize();
        void RenderFrame();

        GeometryBuffer* GetGeometryBuffer(const std::string& name) { return _geoemtryBuffers.find(name)->second.get(); }
        Shader* GetShader(const std::string& name)                 { return _shaders.find(name)->second.get(); }

        ID3D12GraphicsCommandList* GetCommandList();
        DescriptorHeap* GetRootDescriptorHeap();
        void StageBuffer(class UploadBuffer* uploadBuffer);

    private:
        Renderer() = default;
        ~Renderer() = default;

        void InitBuffers();
        void InitShaders();
        void RenderBegin();
        void Render();
        void RenderEnd();
        void RenderImGui();

        FrameContext* _nextFrameCtx = nullptr;
        uint32 _backBufferIndex = 0;

        std::unordered_map<std::string, std::unique_ptr<GeometryBuffer>> _geoemtryBuffers;
        std::unordered_map<std::string, std::unique_ptr<Shader>> _shaders;
    };
} // namespace tb
