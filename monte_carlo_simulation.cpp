#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>

// Parameters for the simulation
struct SimulationParams {
    double S0;           // Initial stock price
    double mu;           // Expected return (annualized)
    double sigma;        // Volatility (annualized)
    double T;            // Time period in years
    int steps;           // Number of time steps
    int num_paths;       // Number of simulation paths
};

// Generate a single path of stock prices using Geometric Brownian Motion
std::vector<double> generatePath(const SimulationParams& params, std::mt19937& gen) {
    std::vector<double> path(params.steps + 1);
    path[0] = params.S0;
    
    // Time step size
    double dt = params.T / params.steps;
    
    // Precompute some values for efficiency
    double drift = (params.mu - 0.5 * params.sigma * params.sigma) * dt;
    double vol = params.sigma * std::sqrt(dt);
    
    // Normal distribution for random shocks
    std::normal_distribution<double> dist(0.0, 1.0);
    
    // Generate the path
    for (int i = 1; i <= params.steps; ++i) {
        double Z = dist(gen);  // Random normal shock
        path[i] = path[i-1] * std::exp(drift + vol * Z);
    }
    
    return path;
}

// Run the Monte Carlo simulation and return all paths
std::vector<std::vector<double>> runMonteCarloSimulation(const SimulationParams& params) {
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Generate multiple paths
    std::vector<std::vector<double>> paths(params.num_paths);
    for (int i = 0; i < params.num_paths; ++i) {
        paths[i] = generatePath(params, gen);
    }
    
    return paths;
}

// Calculate statistics from the simulation results
void calculateStatistics(const std::vector<std::vector<double>>& paths, const SimulationParams& params) {
    int num_paths = paths.size();
    int steps = params.steps;
    
    // Extract final prices
    std::vector<double> final_prices;
    for (const auto& path : paths) {
        final_prices.push_back(path[steps]);
    }
    
    // Calculate mean
    double sum = 0.0;
    for (double price : final_prices) {
        sum += price;
    }
    double mean = sum / num_paths;
    
    // Calculate standard deviation
    double sum_squared_diff = 0.0;
    for (double price : final_prices) {
        sum_squared_diff += (price - mean) * (price - mean);
    }
    double std_dev = std::sqrt(sum_squared_diff / num_paths);
    
    // Calculate min and max
    double min_price = *std::min_element(final_prices.begin(), final_prices.end());
    double max_price = *std::max_element(final_prices.begin(), final_prices.end());
    
    // Calculate percentiles (5% and 95%)
    std::vector<double> sorted_prices = final_prices;
    std::sort(sorted_prices.begin(), sorted_prices.end());
    double percentile_5 = sorted_prices[static_cast<int>(0.05 * num_paths)];
    double percentile_95 = sorted_prices[static_cast<int>(0.95 * num_paths)];
    
    // Print statistics
    std::cout << "\nSimulation Statistics (Final Stock Price):\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Mean: $" << std::fixed << std::setprecision(2) << mean << std::endl;
    std::cout << "Standard Deviation: $" << std::fixed << std::setprecision(2) << std_dev << std::endl;
    std::cout << "Minimum: $" << std::fixed << std::setprecision(2) << min_price << std::endl;
    std::cout << "Maximum: $" << std::fixed << std::setprecision(2) << max_price << std::endl;
    std::cout << "5th Percentile: $" << std::fixed << std::setprecision(2) << percentile_5 << std::endl;
    std::cout << "95th Percentile: $" << std::fixed << std::setprecision(2) << percentile_95 << std::endl;
}

// Save simulation results to CSV files for plotting
void saveResultsToCSV(const std::vector<std::vector<double>>& paths, const SimulationParams& params) {
    // Save all paths to a single CSV file
    std::ofstream all_paths_file("stock_price_paths.csv");
    
    // Write header (time steps)
    all_paths_file << "Path,";
    for (int i = 0; i <= params.steps; ++i) {
        double t = i * (params.T / params.steps);
        all_paths_file << t;
        if (i < params.steps) all_paths_file << ",";
    }
    all_paths_file << std::endl;
    
    // Write each path
    for (int i = 0; i < params.num_paths; ++i) {
        all_paths_file << i + 1 << ",";
        for (int j = 0; j <= params.steps; ++j) {
            all_paths_file << paths[i][j];
            if (j < params.steps) all_paths_file << ",";
        }
        all_paths_file << std::endl;
    }
    
    all_paths_file.close();
    
    // Save time points to a separate file
    std::ofstream time_file("time_points.csv");
    for (int i = 0; i <= params.steps; ++i) {
        double t = i * (params.T / params.steps);
        time_file << t << std::endl;
    }
    time_file.close();
    
    std::cout << "Results saved to CSV files for plotting." << std::endl;
}

// Generate a simple HTML file with embedded JavaScript for plotting
void generatePlotHTML(const SimulationParams& params) {
    std::ofstream html_file("stock_price_plot.html");
    
    html_file << R"(
<!DOCTYPE html>
<html>
<head>
    <title>Monte Carlo Stock Price Simulation</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        .chart-container {
            width: 100%;
            height: 600px;
            margin-top: 20px;
        }
        h1, h2 {
            color: #333;
        }
        .params {
            background-color: #f5f5f5;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Monte Carlo Stock Price Simulation</h1>
        
        <div class="params">
            <h2>Simulation Parameters</h2>
            <p><strong>Initial Stock Price:</strong> $)" << params.S0 << R"(</p>
            <p><strong>Expected Annual Return:</strong> )" << (params.mu * 100) << R"(%</p>
            <p><strong>Annual Volatility:</strong> )" << (params.sigma * 100) << R"(%</p>
            <p><strong>Time Period:</strong> )" << params.T << R"( years</p>
            <p><strong>Number of Paths:</strong> )" << params.num_paths << R"(</p>
        </div>
        
        <div class="chart-container">
            <canvas id="stockChart"></canvas>
        </div>
    </div>

    <script>
        // Function to load CSV data
        async function loadCSV(url) {
            const response = await fetch(url);
            const data = await response.text();
            return data.split('\n').map(row => row.split(','));
        }

        // Function to generate random colors
        function getRandomColor() {
            const letters = '0123456789ABCDEF';
            let color = '#';
            for (let i = 0; i < 6; i++) {
                color += letters[Math.floor(Math.random() * 16)];
            }
            return color;
        }

        // Main function to load data and create chart
        async function createChart() {
            try {
                const pathsData = await loadCSV('stock_price_paths.csv');
                
                // Parse header for time points
                const timePoints = pathsData[0].slice(1).map(parseFloat);
                
                // Create datasets for each path
                const datasets = [];
                
                // Limit to showing only 20 paths for better visualization
                const maxPathsToShow = 20;
                const pathsToShow = Math.min()" << params.num_paths << R"(, maxPathsToShow);
                
                for (let i = 1; i <= pathsToShow; i++) {
                    const pathValues = pathsData[i].slice(1).map(parseFloat);
                    
                    datasets.push({
                        label: `Path ${i}`,
                        data: pathValues,
                        borderColor: getRandomColor(),
                        backgroundColor: 'transparent',
                        borderWidth: 1,
                        pointRadius: 0
                    });
                }
                
                // Create the chart
                const ctx = document.getElementById('stockChart').getContext('2d');
                const stockChart = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: timePoints,
                        datasets: datasets
                    },
                    options: {
                        responsive: true,
                        maintainAspectRatio: false,
                        plugins: {
                            title: {
                                display: true,
                                text: 'Stock Price Simulation Paths',
                                font: {
                                    size: 16
                                }
                            },
                            legend: {
                                display: false
                            },
                            tooltip: {
                                mode: 'index',
                                intersect: false
                            }
                        },
                        scales: {
                            x: {
                                title: {
                                    display: true,
                                    text: 'Time (years)'
                                }
                            },
                            y: {
                                title: {
                                    display: true,
                                    text: 'Stock Price ($)'
                                }
                            }
                        }
                    }
                });
            } catch (error) {
                console.error('Error loading data:', error);
                document.body.innerHTML += `<p style="color: red">Error loading data: ${error.message}</p>`;
            }
        }

        // Call the function when the page loads
        window.onload = createChart;
    </script>
</body>
</html>
    )";
    
    html_file.close();
    
    std::cout << "HTML plot file generated: stock_price_plot.html" << std::endl;
    std::cout << "Open this file in a web browser to view the simulation paths." << std::endl;
}

int main() {
    // Default simulation parameters
    SimulationParams params;
    
    // Get user input for simulation parameters
    std::cout << "Monte Carlo Stock Price Simulation\n";
    std::cout << "==================================\n\n";
    
    std::cout << "Enter initial stock price ($): ";
    std::cin >> params.S0;
    
    std::cout << "Enter expected annual return (as decimal, e.g., 0.08 for 8%): ";
    std::cin >> params.mu;
    
    std::cout << "Enter annual volatility (as decimal, e.g., 0.20 for 20%): ";
    std::cin >> params.sigma;
    
    std::cout << "Enter time period (in years): ";
    std::cin >> params.T;
    
    std::cout << "Enter number of time steps: ";
    std::cin >> params.steps;
    
    std::cout << "Enter number of simulation paths: ";
    std::cin >> params.num_paths;
    
    std::cout << "\nRunning Monte Carlo simulation...\n";
    
    // Run the simulation
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<double>> paths = runMonteCarloSimulation(params);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    // Calculate execution time
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";
    
    // Calculate and display statistics
    calculateStatistics(paths, params);
    
    // Save results to CSV files
    saveResultsToCSV(paths, params);
    
    // Generate HTML plot
    generatePlotHTML(params);
    
    return 0;
}