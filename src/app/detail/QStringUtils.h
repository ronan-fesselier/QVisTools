#pragma once

#include <QString>

#include <string_view>

namespace qvistools::detail
{
    inline QString toQString(std::string_view s)
    {
        return QString::fromUtf8(s);
    }
} // namespace qvistools::detail
