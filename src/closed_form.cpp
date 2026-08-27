#include "closed_form.hpp"

#include <cmath>
#include <stdexcept>

#include "math_functions.hpp"


cf_prices black_scholes(const input& in)
{
    if (!std::isfinite(in.S) || !std::isfinite(in.K) || !std::isfinite(in.T)
        || !std::isfinite(in.v) || !std::isfinite(in.q) || !std::isfinite(in.r)
        || in.S <= 0.0 || in.K <= 0.0 || in.T <= 0.0 || in.v <= 0.0)
    {
        throw std::invalid_argument(
            "spot, strike, maturity, and volatility must be positive");
    }
    const double sqrt_T = std::sqrt(in.T);
    const double d1 = (std::log(in.S / in.K)
        + (in.r - in.q + 0.5 * in.v * in.v) * in.T) / (in.v * sqrt_T);
    const double d2 = d1 - in.v * sqrt_T;
    cf_prices cp;
    cp.cf_call = in.S * std::exp(-in.q * in.T) * norm_cdf(d1)
        - in.K * std::exp(-in.r * in.T) * norm_cdf(d2);
    cp.cf_put = cp.cf_call - in.S * std::exp(-in.q * in.T)
        + in.K * std::exp(-in.r * in.T);
    return cp;
}
