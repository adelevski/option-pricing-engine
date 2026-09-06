# Option Pricing Engine

A lightweight option pricing engine that uses Monte Carlo methods to simulate asset prices for the duration of a given maturity, and uses the results to price call and put contracts. 

Currently implemented:
- European vanilla
- Asian arithmetic fixed-strike
- Asian geometric fixed-strike 

European Monte Carlo estimates are displayed beside Black-Scholes-Merton
closed-form prices so their signed differences can be inspected.

The CLI asks whether to price an Asian or European option, collects the model
inputs, and displays call and put values.

## Build and test

The project requires a C++17 compiler and CMake 3.20 or newer.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/option-pricer
```

The test executable covers payoff functions, arithmetic and geometric path
averages, a known Black-Scholes-Merton result, deterministic seeded Monte Carlo
convergence, and invalid-input handling.

## Numerical scope and limitations

- The model uses risk-neutral geometric Brownian motion with continuous rates,
  volatility, and dividend yield supplied as decimal annualized inputs.
- Monte Carlo output is a point estimate only; the CLI does not report sampling
  error or confidence intervals.
- The Asian estimator retains its legacy monitoring convention:
  `floor(252 * maturity)` observations, starting at time zero and then stepping
  in increments of `1/252` year. Maturities shorter than one observation are
  rejected, and the terminal maturity is not an observation. That schedule
  must be specified before presenting Asian prices as production-grade results.

- Zero volatility is supported as a deterministic discounted payoff.
- Path and simulation averages avoid overflowing their intermediate sum. Models
  that exceed the supported floating-point range raise an explicit error rather
  than returning a nonfinite price. These checks do not establish accuracy for
  extreme inputs.

## Possible future work

- Implement digitals
- Implement double digitals
- Implement American options
- Implement closed form solutions for Asian options
