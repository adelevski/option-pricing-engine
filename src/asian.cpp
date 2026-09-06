#include "asian.hpp"
#include "numerics.hpp"

#include <cmath>
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
    double mean = 0.0;
    std::size_t count = 0;
    for (const double price : S_vec)
    {
        if (!std::isfinite(price) || price < 0.0)
        {
            throw std::invalid_argument("price path values must be finite and nonnegative");
        }
        ++count;
        mean += (price - mean) / static_cast<double>(count);
    }
    return pricing_detail::finite(po_(mean));
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
    const double mean = pricing_detail::positive_exp(sum / static_cast<double>(period));
    return pricing_detail::finite(po_(mean));
}
