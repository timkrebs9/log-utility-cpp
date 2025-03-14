#pragma once

#include "config_parser.hpp"
#include <string>
#include <vector>
#include <chrono>

namespace metrics {
    struct MetricData;

    class MetricProvider {
    public:
        explicit MetricProvider(const MetricConfig& config) : config_(config) {}
        virtual ~MetricProvider() = default;
    
        virtual std::vector<MetricData> collect() = 0;
    
    protected:
        MetricConfig config_;
    };


    // CPU usage provider
    class CpuUsageProvider : public MetricProvider {
    public:
        explicit CpuUsageProvider(const MetricConfig& config);
        std::vector<MetricData> collect() override;
    
    private:
        double last_total_time_ = 0;
        double last_idle_time_ = 0;
    };
    
    // RAM usage provider
    class RamUsageProvider : public MetricProvider {
    public:
        explicit RamUsageProvider(const MetricConfig& config);
        std::vector<MetricData> collect() override;
    };
    
    // Disk usage provider
    class DiskUsageProvider : public MetricProvider {
    public:
        explicit DiskUsageProvider(const MetricConfig& config);
        std::vector<MetricData> collect() override;
    };
    
    // Factory to create providers
    std::unique_ptr<MetricProvider> create_provider(const MetricConfig& config);
    
}