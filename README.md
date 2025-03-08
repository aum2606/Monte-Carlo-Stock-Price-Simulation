# Monte Carlo Stock Price Simulation

This C++ program simulates future stock prices using geometric Brownian motion, a stochastic process commonly used to model stock prices in financial mathematics.

## Features

- Simulates multiple stock price paths using geometric Brownian motion
- Calculates statistics on the simulation results (mean, standard deviation, percentiles)
- Generates interactive HTML plots of the simulated paths
- Saves simulation data to CSV files for further analysis

## Mathematical Model

The simulation uses the geometric Brownian motion model, which assumes that stock prices follow the stochastic differential equation:

dS = μS dt + σS dW

Where:
- S is the stock price
- μ is the expected return (drift)
- σ is the volatility
- dW is a Wiener process (random Brownian motion)

The solution to this equation gives the stock price at time t:

S(t) = S(0) * exp((μ - σ²/2)t + σW(t))

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- Web browser for viewing the generated plots

## Building the Project

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Running the Simulation

After building, run the executable:

```bash
./Release/monte_carlo_simulation.exe
```

You'll be prompted to enter the following parameters:
- Initial stock price
- Expected annual return (as a decimal, e.g., 0.08 for 8%)
- Annual volatility (as a decimal, e.g., 0.20 for 20%)
- Time period in years
- Number of time steps
- Number of simulation paths

## Example Parameters

For a stock with:
- Initial price: $100
- Expected annual return: 8% (0.08)
- Annual volatility: 20% (0.20)
- Time period: 1 year
- 252 time steps (trading days in a year)
- 1000 simulation paths

## Viewing the Results

After the simulation completes:
1. Statistics will be displayed in the console
2. CSV files will be generated with the simulation data
3. An HTML file (`stock_price_plot.html`) will be created
4. Open the HTML file in a web browser to view the interactive plot
