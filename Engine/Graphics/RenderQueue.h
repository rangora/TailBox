#pragma once

#include "Core.h"
#include "DrawCall.h"

namespace tb
{
    class RenderQueue
    {
    public:
        RenderQueue() = default;
        ~RenderQueue() = default;

        void Submit(const DrawCall& drawCall);
        void Clear();

        const std::vector<DrawCall>& GetDrawCalls() const { return _drawCalls; }

    private:
        std::vector<DrawCall> _drawCalls;
    };
} // namespace tb
