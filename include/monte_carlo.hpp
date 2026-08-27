#pragma once

#include <cstdint>

#include "input.hpp"


struct sim_prices
{
    double sim_call;
    double sim_put;
};

sim_prices euro_monte_carlo(const input& in);
sim_prices euro_monte_carlo(const input& in, std::uint32_t seed);

sim_prices asian_monte_carlo(const asian_input& in);
sim_prices asian_monte_carlo(const asian_input& in, std::uint32_t seed);
