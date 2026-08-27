#include "math_functions.hpp"

#include <cmath>


double norm_cdf(double x)
{
    return std::erfc(-x / std::sqrt(2.0)) / 2.0;
}
