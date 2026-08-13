#include "RenderPassForwardRendering.h"
#include "GraphicsCore.h"
#include "Material.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "MemoryAllocator.h"
#include "Engine.h"

namespace tb
{
    RenderPassForwardRendering::~RenderPassForwardRendering()
    {
    }

    void RenderPassForwardRendering::Render(const D3D12View& view)
    {
        ID3D12GraphicsCommandList* cmdList = g_commandContext.GetCommandList();

        g_commandContext.SetRootSignature("Material");
        cmdList->SetPipelineState(g_renderer.GetPipelineState("Material").Get());

        GlobalConstants globalConst;
        XMMATRIX viewProjMtx = view._sceneViewMtx * view._projMtx;
        XMStoreFloat4x4(&globalConst._viewProj, XMMatrixTranspose(viewProjMtx));
        globalConst._cameraPosition = Engine::GetActiveCameraPosition();
        globalConst._time = 0.f;

        for (const DrawCall& drawCall : g_renderQueue.GetDrawCalls())
        {
            MeshConstants meshConst;
            XMStoreFloat4x4(&meshConst._worldMtx, drawCall._worldMtx);

            MaterialConstants materialConst = {};
            if (drawCall._material)
            {
                drawCall._material->UpdateMaterialConstantBuffer(materialConst);
            }

            CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = {};
            CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = {};

            if (!g_commandContext._descriptorPool->AllocDescriptor(&cpuHandle, &gpuHandle, 1))
            {
                continue;
            }

            CBBlock* meshCB = g_commandContext._constantBufferPool->Allocate();
            CBBlock* globalCB = g_commandContext._constantBufferPool->Allocate();
            CBBlock* materialCB = g_commandContext._constantBufferPool->Allocate();
            if (!meshCB || !globalCB || !materialCB)
            {
                continue;
            }

            *reinterpret_cast<MeshConstants*>(meshCB->_cpuMemAddr) = meshConst;
            *reinterpret_cast<GlobalConstants*>(globalCB->_cpuMemAddr) = globalConst;
            *reinterpret_cast<MaterialConstants*>(materialCB->_cpuMemAddr) = materialConst;

            cmdList->SetGraphicsRootConstantBufferView(0, meshCB->_gpuMemAddr);
            cmdList->SetGraphicsRootConstantBufferView(1, globalCB->_gpuMemAddr);
            cmdList->SetGraphicsRootConstantBufferView(2, materialCB->_gpuMemAddr);

            ID3D12DescriptorHeap* descriptorHeap = g_commandContext._descriptorPool->GetDescriptorHeap();
            cmdList->SetDescriptorHeaps(1, &descriptorHeap);

            if (drawCall._material)
            {
                TextureResource* textureResource = drawCall._material->GetTextureResource(TextureType::BASECOLOR);
                if (textureResource && textureResource->_srvCpuHandle.ptr)
                {
                    g_renderAPI->CopyTextureDescriptor(cpuHandle, textureResource->_srvCpuHandle);
                }
            }

            cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            cmdList->SetGraphicsRootDescriptorTable(3, gpuHandle);

            g_renderAPI->Draw(drawCall._meshId);
        }

        g_renderQueue.Clear();
    }
} // namespace tb
