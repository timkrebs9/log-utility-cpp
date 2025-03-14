#include <chrono>
#include <filesystem>
#include <iostream>
#include <signal.h>
#include <thread>

#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "azure_uploader.hpp"
#include "config_parser.hpp"
#include "metric_collector.hpp"
#include "metric_provider.hpp"


namespace fs = std::filesystem;

// Global variables for signal handling
static bool running = true;

// Signal handler for graceful shutdown
void signal_handler(int signal)
{
    spdlog::info("Received signal {}. Shutting down...", signal);
    running = false;
}

int main(int argc, char **argv)
{
    // Parse command-line arguments
    cxxopts::Options options("metrics_collector",
                             "System metrics collector for Azure Storage");

    options.add_options()(
        "c,config",
        "Configuration file path",
        cxxopts::value<std::string>()->default_value(
            "/mnt/d/Development/CPP/backup-utility-cpp/app/config.json"))(
        "v,verbose",
        "Verbose output",
        cxxopts::value<bool>()->default_value("false"))("h,help",
                                                        "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    // Set log level
    bool verbose = result["verbose"].as<bool>();
    spdlog::set_level(verbose ? spdlog::level::debug : spdlog::level::info);

    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try
    {
        // Load configuration
        std::string config_path = result["config"].as<std::string>();
        spdlog::info("Loading configuration from: {}", config_path);

        metrics::ConfigParser config(config_path);

        // Create metrics collector
        metrics::MetricCollector collector(config);

        // Create Azure uploader
        metrics::AzureUploader uploader(config.get_azure_connection_string(),
                                        config.get_azure_container_name());

        // Start collecting metrics
        collector.start();

        spdlog::info("Metrics collector started. Press Ctrl+C to stop.");

        // Main loop
        while (running)
        {
            // Sleep for a while
            std::this_thread::sleep_for(
                std::chrono::seconds(config.get_collection_interval_seconds()));

            // Get metrics and upload them
            auto metrics_data = collector.get_metrics_and_clear();
            if (!metrics_data.empty())
            {
                if (uploader.upload_metrics(metrics_data))
                {
                    spdlog::info("Successfully uploaded {} metrics",
                                 metrics_data.size());
                }
                else
                {
                    spdlog::error("Failed to upload metrics");
                }
            }
        }

        // Stop the collector
        collector.stop();
    }
    catch (const std::exception &ex)
    {
        spdlog::critical("Error: {}", ex.what());
        return 1;
    }

    spdlog::info("Metrics collector shutdown complete.");
    return 0;
}
