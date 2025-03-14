#include "metric_collector.hpp"
#include "metric_provider.hpp"
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>

namespace metrics {

MetricCollector::MetricCollector(const ConfigParser& config)
    : collection_interval_seconds_(config.get_collection_interval_seconds()) {
    register_providers(config.get_enabled_metrics());
}

MetricCollector::~MetricCollector() {
    stop();
}

void MetricCollector::register_providers(const std::vector<MetricConfig>& metrics) {
    for (const auto& metric_config : metrics) {
        auto provider = create_provider(metric_config);
        if (provider) {
            providers_.push_back(std::move(provider));
        }
    }
    spdlog::info("Registered {} metric providers", providers_.size());
}

void MetricCollector::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    collection_thread_ = std::thread(&MetricCollector::collection_routine, this);
    spdlog::info("Metrics collection started");
}

void MetricCollector::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (collection_thread_.joinable()) {
        collection_thread_.join();
    }
    spdlog::info("Metrics collection stopped");
}

void MetricCollector::collection_routine() {
    while (running_) {
        collect_metrics();
        
        // Sleep for the specified interval
        std::this_thread::sleep_for(std::chrono::seconds(collection_interval_seconds_));
    }
}

void MetricCollector::collect_metrics() {
    std::vector<MetricData> new_metrics;
    
    for (auto& provider : providers_) {
        try {
            auto metrics = provider->collect();
            new_metrics.insert(new_metrics.end(), metrics.begin(), metrics.end());
        } catch (const std::exception& ex) {
            spdlog::error("Error collecting metrics: {}", ex.what());
        }
    }
    
    // Add the new metrics to the buffer
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    metrics_buffer_.insert(metrics_buffer_.end(), new_metrics.begin(), new_metrics.end());
}

nlohmann::json MetricCollector::get_metrics_and_clear() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    
    nlohmann::json result = nlohmann::json::array();
    
    for (const auto& metric : metrics_buffer_) {
        auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            metric.timestamp.time_since_epoch()).count();
            
        nlohmann::json metric_json = {
            {"name", metric.name},
            {"unit", metric.unit},
            {"value", metric.value},
            {"timestamp", timestamp_ms}
        };
        
        result.push_back(metric_json);
    }
    
    metrics_buffer_.clear();
    return result;
}

} // namespace metrics