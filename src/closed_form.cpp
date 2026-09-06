#include "closed_form.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "math_functions.hpp"
#include "numerics.hpp"


cf_prices black_scholes(const input& in)
{
    if (!std::isfinite(in.S) || !std::isfinite(in.K) || !std::isfinite(in.T)
        || !std::isfinite(in.v) || !std::isfinite(in.q) || !std::isfinite(in.r)
        || in.S <= 0.0 || in.K <= 0.0 || in.T <= 0.0 || in.v < 0.0)
    {
        throw std::invalid_argument(
            "spot, strike, and maturity must be positive; volatility must be nonnegative");
    }
    const double discounted_spot = pricing_detail::finite(
        in.S * pricing_detail::positive_exp(-in.q * in.T));
    const double discounted_strike = pricing_detail::finite(
        in.K * pricing_detail::positive_exp(-in.r * in.T));
    if (in.v == 0.0)
    {
        return {std::max(discounted_spot - discounted_strike, 0.0),
            std::max(discounted_strike - discounted_spot, 0.0)};
    }
    const double vol_T = pricing_detail::finite(in.v * std::sqrt(in.T));
    const double d1 = pricing_detail::finite((std::log(in.S) - std::log(in.K)
        + (in.r - in.q + 0.5 * in.v * in.v) * in.T) / vol_T);
    const double d2 = pricing_detail::finite(d1 - vol_T);
    cf_prices cp;
    cp.cf_call = pricing_detail::finite(discounted_spot * norm_cdf(d1)
        - discounted_strike * norm_cdf(d2));
    cp.cf_put = pricing_detail::finite(discounted_strike * norm_cdf(-d2)
        - discounted_spot * norm_cdf(-d1));
    return cp;
}
