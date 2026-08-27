#pragma once

#include <vector>

#include "payoff.hpp"


class asian_option
{
public:
    virtual ~asian_option() = default;
    virtual double payoff_price(const std::vector<double>& S_vec) const = 0;
};


class arithmetic_asian : public asian_option
{
public:
    arithmetic_asian(payoff& po);
    double payoff_price(const std::vector<double>& S_vec) const override;

private:
    const payoff& po_;
};


class geometric_asian : public asian_option 
{
public:
    geometric_asian(payoff& po);
    double payoff_price(const std::vector<double>& S_vec) const override;

private:
    const payoff& po_;
};
