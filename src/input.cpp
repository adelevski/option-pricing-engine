#include "input.hpp"

#include <iostream>
#include <stdexcept>


namespace
{
template <typename T>
void read_value(const char* prompt, T& value)
{
    std::cout << prompt;
    if (!(std::cin >> value))
    {
        throw std::invalid_argument("input must match the requested type");
    }
}
}


input get_simple_input()
{
    input in{};
    char choice{};
    read_value("Debug input? [y/n]: ", choice);
    if (choice == 'y')
    {
        in.S = 100;
        in.K = 100;
        in.T = 1.0;
        in.v = 0.2;
        in.q = 0.0;
        in.r = 0.05;
        in.num_sims = 1e5;
    }
    else
    {
        read_value("Spot price?: ", in.S);
        read_value("Strike price?: ", in.K);
        read_value("Years?: ", in.T);
        read_value("Volatility?: ", in.v);
        read_value("Dividend yield?: ", in.q);
        read_value("RFR?: ", in.r);
        read_value("Simulations?: ", in.num_sims);
    }
	std::cout << "One moment please!\n" << std::endl;
    return in;
}


asian_input get_asian_input()
{
    asian_input in{};
    char choice{};
    read_value("Arithmetic or Geometric pricing? [a/g]: ", choice);
    if (choice == 'a')
    {
        in.type = 'a';
    }
    else if (choice == 'g')
    {
        in.type = 'g';
    }
    else
    {
        std::cout << "Improper input, setting to arithmetic." << std::endl;
        in.type = 'a';
    }
    read_value("Spot price?: ", in.S);
    read_value("Strike price?: ", in.K);
    read_value("Years?: ", in.T);
    read_value("Volatility?: ", in.v);
    read_value("Dividend yield?: ", in.q);
    read_value("RFR?: ", in.r);
    read_value("Simulations?: ", in.num_sims);
    std::cout << "One moment please!\n" << std::endl;
    return in;
}
