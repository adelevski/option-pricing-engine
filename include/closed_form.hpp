#pragma once

#include "input.hpp"


struct cf_prices
{
    double cf_call;
    double cf_put;
};

cf_prices black_scholes(const input& in);
