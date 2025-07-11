#pragma once

#include "Core.h"

namespace tb
{
    struct InputLayout
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;

        void AddElement(const char* semantic, UINT index, DXGI_FORMAT format, UINT offset, UINT slot,
                        D3D12_INPUT_CLASSIFICATION classification, UINT stepRate);

        UINT GetSize() const;
        D3D12_INPUT_LAYOUT_DESC GetDesc() const;
        D3D12_INPUT_ELEMENT_DESC* GetPointer() { return _elements.data(); }

        static std::vector<std::string> semanticStorage;
    };

    struct InputLayoutPreset
    {
        static InputLayout BaseInputLayout();
    };
}