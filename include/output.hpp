#pragma once

#include "input.hpp"
#include "monte_carlo.hpp"
#include "closed_form.hpp"


void print_simple_results(
    const input& in,
    const sim_prices& sp,
    const cf_prices& cp
);


void print_asian_results(
    const asian_input& in,
    const sim_prices& sp
);
