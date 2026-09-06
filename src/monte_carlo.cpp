#include "monte_carlo.hpp"

#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

#include "asian.hpp"
#include "payoff.hpp"
#include "numerics.hpp"


namespace
{
void validate_simulation_input(const input& in)
{
    if (!std::isfinite(in.S) || !std::isfinite(in.K) || !std::isfinite(in.T)
        || !std::isfinite(in.v) || !std::isfinite(in.q) || !std::isfinite(in.r)
        || in.S <= 0.0 || in.K <= 0.0 || in.T <= 0.0 || in.v < 0.0)
    {
        throw std::invalid_argument(
            "spot, strike, and maturity must be positive; volatility must be nonnegative");
    }
    if (in.num_sims <= 0)
    {
        throw std::invalid_argument("number of simulations must be positive");
    }
}
}

sim_prices euro_monte_carlo(const input& in)
{
    std::random_device random_device;
    return euro_monte_carlo(in, static_cast<std::uint32_t>(random_device()));
}

sim_prices euro_monte_carlo(const input& in, std::uint32_t seed)
{
    validate_simulation_input(in);
    double call_mean = 0.0;
    double put_mean = 0.0;
    const double drift = pricing_detail::positive_exp((in.r - in.q - 0.5 * in.v * in.v) * in.T);
    const double S_adjusted = pricing_detail::finite(in.S * drift);
    const double vol_dt = pricing_detail::finite(in.v * std::sqrt(in.T));

    call_payoff call_po(in.K);
    put_payoff put_po(in.K);

    std::mt19937 gen(seed);
    std::normal_distribution<> d(0,1);

    for (int i = 0; i < in.num_sims; i++)
    {
        const double shock = pricing_detail::positive_exp(vol_dt * d(gen));
        const double S_current = pricing_detail::finite(S_adjusted * shock);
        call_mean += (call_po(S_current) - call_mean) / (i + 1);
        put_mean += (put_po(S_current) - put_mean) / (i + 1);
    }
    sim_prices sp;
    sp.sim_call = pricing_detail::finite(pricing_detail::positive_exp(-in.r * in.T) * call_mean);
    sp.sim_put = pricing_detail::finite(pricing_detail::positive_exp(-in.r * in.T) * put_mean);
    return sp;
}



sim_prices asian_monte_carlo(const asian_input& in)
{
    std::random_device random_device;
    return asian_monte_carlo(in, static_cast<std::uint32_t>(random_device()));
}

sim_prices asian_monte_carlo(const asian_input& in, std::uint32_t seed)
{
    validate_simulation_input(in);
    if (in.type != 'a' && in.type != 'g')
    {
        throw std::invalid_argument("Asian option type must be 'a' or 'g'");
    }
    double call_mean = 0.0;
    double put_mean = 0.0;
    constexpr double dt = 1.0 / 252.0;
    const long double observation_count =
        std::floor(static_cast<long double>(in.T) * 252.0L);
    if (!std::isfinite(observation_count)
        || observation_count < 1.0L
        || observation_count
            >= static_cast<long double>(std::numeric_limits<std::size_t>::max()))
    {
        throw std::invalid_argument(
            "maturity must produce a representable Asian observation count");
    }
    const auto period = static_cast<std::size_t>(observation_count);
    const double drift = pricing_detail::positive_exp((in.r - in.q - 0.5 * in.v * in.v) * dt);
    const double vol_dt = pricing_detail::finite(in.v * std::sqrt(dt));
    std::vector<double> S_vec(period);

    call_payoff call_po(in.K);
    put_payoff put_po(in.K);
    std::unique_ptr<asian_option> asian_call;
    std::unique_ptr<asian_option> asian_put;

    if (in.type == 'a')
    {
        asian_call = std::make_unique<arithmetic_asian>(call_po);
        asian_put = std::make_unique<arithmetic_asian>(put_po);
    }
    else
    {
        asian_call = std::make_unique<geometric_asian>(call_po);
        asian_put = std::make_unique<geometric_asian>(put_po);
    }

    std::mt19937 gen(seed);
    std::normal_distribution<> d(0, 1);

    for (int i = 0; i < in.num_sims; i++)
    {
        S_vec[0] = in.S;

        for (std::size_t j = 1; j < period; ++j)
        {
            const double shock = pricing_detail::positive_exp(vol_dt * d(gen));
            S_vec[j] = pricing_detail::finite(S_vec[j - 1] * drift * shock);
        }

        call_mean += (asian_call->payoff_price(S_vec) - call_mean) / (i + 1);
        put_mean += (asian_put->payoff_price(S_vec) - put_mean) / (i + 1);
    }
    sim_prices sp;
    sp.sim_call = pricing_detail::finite(pricing_detail::positive_exp(-in.r * in.T) * call_mean);
    sp.sim_put = pricing_detail::finite(pricing_detail::positive_exp(-in.r * in.T) * put_mean);
    return sp;
}
