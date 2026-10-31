#pragma once

#include <QMetaType>

namespace dnv::vista::sdk
{
    class GmodNode;
} // namespace dnv::vista::sdk

Q_DECLARE_OPAQUE_POINTER(const dnv::vista::sdk::GmodNode*)
Q_DECLARE_METATYPE(const dnv::vista::sdk::GmodNode*)
