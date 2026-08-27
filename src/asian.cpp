#include "asian.hpp"

#include <cmath>
#include <numeric>
#include <stdexcept>



arithmetic_asian::arithmetic_asian(payoff& po)
    : po_(po)
{}

double arithmetic_asian::payoff_price(const std::vector<double>& S_vec) const
{
    if (S_vec.empty())
    {
        throw std::invalid_argument("price path must not be empty");
    }
    const auto period = S_vec.size();
    for (const double price : S_vec)
    {
        if (!std::isfinite(price))
        {
            throw std::invalid_argument("price path values must be finite");
        }
    }
    const double sum = std::accumulate(S_vec.begin(), S_vec.end(), 0.0);
    const double mean = sum / static_cast<double>(period);
    return po_(mean);
}


geometric_asian::geometric_asian(payoff& po)
    : po_(po)
{}

double geometric_asian::payoff_price(const std::vector<double>& S_vec) const
{
    if (S_vec.empty())
    {
        throw std::invalid_argument("price path must not be empty");
    }
    const auto period = S_vec.size();
    double sum = 0.0;
    for (const double price : S_vec)
    {
        if (!std::isfinite(price) || price <= 0.0)
        {
            throw std::invalid_argument("price path values must be positive");
        }
        sum += std::log(price);
    }
    const double mean = std::exp(sum / static_cast<double>(period));
    return po_(mean);
}
