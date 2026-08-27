#include "output.hpp"

#include <iostream>
#include <iomanip>
#include <string>


void print_simple_results(
    const input& in,
    const sim_prices& sp,
    const cf_prices& cp)
{
    double call_error = cp.cf_call - sp.sim_call;
    double put_error = cp.cf_put - sp.sim_put;

    std::cout << std::setprecision(4) << std::fixed;
    std::cout << "Using " << in.num_sims << " simulations..."               << std::endl;
    std::cout << "================================="                        << std::endl;
    std::cout << "Method      |   call   |   put   "                        << std::endl;
    std::cout << "---------------------------------"                        << std::endl;
    std::cout << "Simulation  |  " << sp.sim_call   << " |  " << sp.sim_put << std::endl;
    std::cout << "Closed form |  " << cp.cf_call    << " |  " << cp.cf_put  << std::endl;
    std::cout << "---------------------------------"                        << std::endl;
    std::cout << "Error:         " << call_error    << "    " << put_error  << std::endl;
    std::cout << "================================="                        << std::endl;
}


void print_asian_results(
    const asian_input& in,
    const sim_prices& sp)
{
    std::string type;
    if (in.type == 'a')
    {
        type = "Arithmetic";
    }
    else
    {
        type = "Geometric";
    }

    std::cout << std::setprecision(4) << std::fixed;
    std::cout << "Using " << in.num_sims << " simulations..."               << std::endl;
    std::cout << "================================="                        << std::endl;
    std::cout << type << " Fixed-Strike Value: "                            << std::endl;
    std::cout << "---------------------------------"                        << std::endl;
    std::cout << "Call: " << sp.sim_call                                    << std::endl;
    std::cout << "Put: " << sp.sim_put                                      << std::endl;
    std::cout << "================================="                        << std::endl;
}
