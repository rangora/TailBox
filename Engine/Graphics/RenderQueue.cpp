#include "RenderQueue.h"

namespace tb
{
    void RenderQueue::Submit(const DrawCall& drawCall)
    {
        _drawCalls.push_back(drawCall);
    }

    void RenderQueue::Clear()
    {
        _drawCalls.clear();
    }
} // namespace tb
