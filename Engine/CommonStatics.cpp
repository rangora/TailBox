#include "CommonStatics.h"

namespace tb
{
    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty())
            return std::wstring();

        int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
        if (len <= 0)
            return std::wstring();

        std::wstring result(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], len);
        return result;
    }
} // namespace tb