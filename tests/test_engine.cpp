#include "asian.hpp"
#include "closed_form.hpp"
#include "monte_carlo.hpp"
#include "payoff.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

static_assert(!std::is_constructible_v<arithmetic_asian, call_payoff&&>);
static_assert(!std::is_constructible_v<geometric_asian, call_payoff&&>);

namespace
{
bool near(double actual, double expected, double tolerance = 1e-6)
{
    return std::abs(actual - expected) <= tolerance;
}

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}
}

int main()
{
    call_payoff call(100.0);
    put_payoff put(100.0);
    require(near(call(120.0), 20.0), "in-the-money call payoff");
    require(near(call(80.0), 0.0), "out-of-the-money call payoff");
    require(near(put(80.0), 20.0), "in-the-money put payoff");
    require(near(put(120.0), 0.0), "out-of-the-money put payoff");

    arithmetic_asian arithmetic(call);
    geometric_asian geometric(call);
    const std::vector<double> path{100.5, 101.5};
    require(near(arithmetic.payoff_price(path), 1.0), "arithmetic average");
    require(
        near(geometric.payoff_price(path), std::sqrt(100.5 * 101.5) - 100.0),
        "geometric average");

    const input vanilla{100.0, 100.0, 1.0, 0.2, 0.0, 0.05, 100000};
    const cf_prices prices = black_scholes(vanilla);
    require(near(prices.cf_call, 10.4506, 1e-3), "Black-Scholes call");
    require(near(prices.cf_put, 5.5735, 1e-3), "Black-Scholes put");

    const sim_prices simulated = euro_monte_carlo(vanilla, 42U);
    const sim_prices repeated = euro_monte_carlo(vanilla, 42U);
    require(std::isfinite(simulated.sim_call), "finite simulated call");
    require(std::isfinite(simulated.sim_put), "finite simulated put");
    require(simulated.sim_call >= 0.0, "nonnegative simulated call");
    require(simulated.sim_put >= 0.0, "nonnegative simulated put");
    require(near(simulated.sim_call, repeated.sim_call, 0.0), "deterministic seeded call");
    require(near(simulated.sim_put, repeated.sim_put, 0.0), "deterministic seeded put");
    require(near(simulated.sim_call, prices.cf_call, 0.15), "Monte Carlo call convergence");
    require(near(simulated.sim_put, prices.cf_put, 0.15), "Monte Carlo put convergence");

    asian_input asian{};
    static_cast<input&>(asian) = vanilla;
    asian.num_sims = 100;
    asian.type = 'g';
    const sim_prices asian_simulated = asian_monte_carlo(asian, 7U);
    const sim_prices asian_repeated = asian_monte_carlo(asian, 7U);
    require(near(asian_simulated.sim_call, asian_repeated.sim_call, 0.0), "deterministic seeded Asian call");
    require(near(asian_simulated.sim_put, asian_repeated.sim_put, 0.0), "deterministic seeded Asian put");

    bool rejected_invalid_input = false;
    try
    {
        const input invalid{100.0, 100.0, 0.0, 0.2, 0.0, 0.05, 100};
        (void)euro_monte_carlo(invalid);
    }
    catch (const std::invalid_argument&)
    {
        rejected_invalid_input = true;
    }
    require(rejected_invalid_input, "invalid input rejection");

    bool rejected_nonfinite_input = false;
    try
    {
        const input invalid{100.0, 100.0, 1.0, 0.2, 0.0,
            std::numeric_limits<double>::quiet_NaN(), 100};
        (void)euro_monte_carlo(invalid, 42U);
    }
    catch (const std::invalid_argument&)
    {
        rejected_nonfinite_input = true;
    }
    require(rejected_nonfinite_input, "nonfinite input rejection");

    bool rejected_nonfinite_closed_form_input = false;
    try
    {
        const input invalid{100.0, 100.0, 1.0, 0.2, 0.0,
            std::numeric_limits<double>::infinity(), 100};
        (void)black_scholes(invalid);
    }
    catch (const std::invalid_argument&)
    {
        rejected_nonfinite_closed_form_input = true;
    }
    require(rejected_nonfinite_closed_form_input, "nonfinite closed-form input rejection");

    bool rejected_default_input = false;
    try
    {
        (void)euro_monte_carlo(input{}, 42U);
    }
    catch (const std::invalid_argument&)
    {
        rejected_default_input = true;
    }
    require(rejected_default_input, "default input rejection");

    bool rejected_empty_path = false;
    try
    {
        (void)arithmetic.payoff_price({});
    }
    catch (const std::invalid_argument&)
    {
        rejected_empty_path = true;
    }
    require(rejected_empty_path, "empty path rejection");

    bool rejected_nonpositive_geometric_path = false;
    try
    {
        (void)geometric.payoff_price({100.0, 0.0});
    }
    catch (const std::invalid_argument&)
    {
        rejected_nonpositive_geometric_path = true;
    }
    require(rejected_nonpositive_geometric_path, "nonpositive geometric path rejection");

    bool rejected_nonfinite_path = false;
    try
    {
        (void)arithmetic.payoff_price(
            {100.0, std::numeric_limits<double>::quiet_NaN()});
    }
    catch (const std::invalid_argument&)
    {
        rejected_nonfinite_path = true;
    }
    require(rejected_nonfinite_path, "nonfinite path rejection");

    bool rejected_excessive_observation_count = false;
    try
    {
        asian_input excessive = asian;
        excessive.T = std::numeric_limits<double>::max();
        (void)asian_monte_carlo(excessive, 7U);
    }
    catch (const std::invalid_argument&)
    {
        rejected_excessive_observation_count = true;
    }
    require(
        rejected_excessive_observation_count,
        "excessive Asian observation count rejection");

    bool rejected_subdaily_maturity = false;
    try
    {
        asian_input subdaily = asian;
        subdaily.T = 0.5 / 252.0;
        (void)asian_monte_carlo(subdaily, 7U);
    }
    catch (const std::invalid_argument&)
    {
        rejected_subdaily_maturity = true;
    }
    require(rejected_subdaily_maturity, "subdaily Asian maturity rejection");
    const input deterministic{100.0, 100.0, 1.0, 0.0, 0.02, 0.05, 10};
    const auto deterministic_cf = black_scholes(deterministic);
    const auto deterministic_mc = euro_monte_carlo(deterministic, 42U);
    require(near(deterministic_cf.cf_call, 100.0 * std::exp(-0.02)
        - 100.0 * std::exp(-0.05)), "zero-volatility discounted payoff");
    require(near(deterministic_cf.cf_call, deterministic_mc.sim_call),
        "zero-volatility Monte Carlo agrees with closed form");
    require(near(deterministic_cf.cf_put, 0.0), "zero-volatility put");

    const input large_spot{1e308, 100.0, 1.0, 0.0, 0.0, 0.0, 10};
    require(near(euro_monte_carlo(large_spot, 42U).sim_call / 1e308, 1.0),
        "Monte Carlo mean does not overflow when the result is representable");
    require(near(arithmetic.payoff_price({1e308, 1e308}) / 1e308, 1.0),
        "arithmetic average does not overflow when the result is representable");

    const input overflowing_discount{100.0, 100.0, 1.0, 0.2, 0.0, -1000.0, 1};
    for (int method = 0; method < 3; ++method)
    {
        bool rejected_numeric_range = false;
        try
        {
            if (method == 0) (void)black_scholes(overflowing_discount);
            else if (method == 1) (void)euro_monte_carlo(overflowing_discount, 42U);
            else
            {
                asian_input excessive_discount{};
                static_cast<input&>(excessive_discount) = overflowing_discount;
                excessive_discount.type = 'a';
                (void)asian_monte_carlo(excessive_discount, 42U);
            }
        }
        catch (const std::overflow_error&)
        {
            rejected_numeric_range = true;
        }
        require(rejected_numeric_range, "out-of-range models fail explicitly");
    }

}
