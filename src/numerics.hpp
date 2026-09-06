#pragma once

#include <cmath>
#include <stdexcept>

namespace pricing_detail
{
inline double finite(double value)
{
    if (!std::isfinite(value))
    {
        throw std::overflow_error("model exceeds the supported numeric range");
    }
    return value;
}

inline double positive_exp(double exponent)
{
    const double value = finite(std::exp(finite(exponent)));
    if (value == 0.0)
    {
        throw std::overflow_error("model exponential underflows the supported numeric range");
    }
    return value;
}
}
