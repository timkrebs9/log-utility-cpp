#pragma once

#include "config_parser.hpp"
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <vector>


namespace metrics
{
// forward Declaration
class MetricProvider;

struct MetricData
{
    std::string name;
    std::string unit;
    double value;
    std::chrono::system_clock::time_point timestamp;
};

class MetricCollector
{
public:
    explicit MetricCollector(const ConfigParser &config);
    ~MetricCollector();

    // Start collecting metrics in a separate thread
    void start();

    // Stop collecting metrics
    void stop();

    // Get collected metrics and clear the internal buffer
    nlohmann::json get_metrics_and_clear();

private:
    std::vector<std::unique_ptr<MetricProvider>> providers_;
    std::vector<MetricData> metrics_buffer_;
    std::mutex buffer_mutex_;
    bool running_ = false;
    int collection_interval_seconds_;
    std::thread collection_thread_;

    void collection_routine();
    void collect_metrics();
    void register_providers(const std::vector<MetricConfig> &metrics);
};
} // namespace metrics
