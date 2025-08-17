#include "Shader.h"

namespace tb
{
    Shader::Shader() = default;

    Shader::~Shader()
    {
        if (_bytecode)
        {
            _bytecode.Reset();
        }
    }
} // namespace tb
