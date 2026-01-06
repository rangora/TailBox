#pragma once

#include "Core.h"

namespace tb
{
    class EditorRenderer
    {
    public:
        static void Create();
        static EditorRenderer* Get() { return _instance; }

        void Initialize();

        void RenderBegin();
        void Render();
        void RenderEnd();

        void Flush();
        void Signal();

        ID3D12GraphicsCommandList* GetCommmandList() const { return _commandList.Get(); }

    private:
        EditorRenderer();
        ~EditorRenderer();

        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<ID3D12CommandAllocator> _commandAllocator = nullptr;

        uint32 _rw = 0;
        uint32 _rh = 0;

        uint64 _fenceLastSignaledValue = 0;

        class D3D12RenderAPI* _renderAPI = nullptr;

        static EditorRenderer* _instance;
    };
} // namespace tb