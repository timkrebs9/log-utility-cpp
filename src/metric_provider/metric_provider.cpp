#include "metric_provider.hpp"
#include "metric_collector.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/statvfs.h>
#include <spdlog/spdlog.h>

namespace metrics {

// CPU Usage Implementation
CpuUsageProvider::CpuUsageProvider(const MetricConfig& config)
    : MetricProvider(config) {}

std::vector<MetricData> CpuUsageProvider::collect() {
    std::vector<MetricData> result;
    
    // Read /proc/stat
    std::ifstream proc_stat("/proc/stat");
    std::string line;
    
    if (std::getline(proc_stat, line)) {
        std::istringstream ss(line);
        std::string cpu;
        double user, nice, system, idle, iowait, irq, softirq, steal;
        
        ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        
        double idle_time = idle + iowait;
        double total_time = user + nice + system + idle + iowait + irq + softirq + steal;
        
        // Calculate CPU usage percentage
        double usage = 0;
        if (last_total_time_ > 0) {
            double delta_idle = idle_time - last_idle_time_;
            double delta_total = total_time - last_total_time_;
            usage = 100.0 * (1.0 - delta_idle / delta_total);
        }
        
        last_idle_time_ = idle_time;
        last_total_time_ = total_time;
        
        MetricData data{
            "cpu_usage",
            "%",
            usage,
            std::chrono::system_clock::now()
        };
        
        result.push_back(data);
    }
    
    return result;
}

// RAM Usage Implementation
RamUsageProvider::RamUsageProvider(const MetricConfig& config)
    : MetricProvider(config) {}
    
std::vector<MetricData> RamUsageProvider::collect() {
    std::vector<MetricData> result;
    
    // Read /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long total_mem = 0, free_mem = 0, buffers = 0, cached = 0;
    
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream(line.substr(line.find(":") + 1)) >> total_mem;
        } else if (line.find("MemFree:") == 0) {
            std::istringstream(line.substr(line.find(":") + 1)) >> free_mem;
        } else if (line.find("Buffers:") == 0) {
            std::istringstream(line.substr(line.find(":") + 1)) >> buffers;
        } else if (line.find("Cached:") == 0) {
            std::istringstream(line.substr(line.find(":") + 1)) >> cached;
        }
    }
    
    // Calculate RAM usage
    long used_mem = total_mem - free_mem - buffers - cached;
    double usage_percent = 100.0 * static_cast<double>(used_mem) / static_cast<double>(total_mem);
    
    MetricData usage_data{
        "ram_usage_percent",
        "%",
        usage_percent,
        std::chrono::system_clock::now()
    };
    
    MetricData total_data{
        "ram_total",
        "KB",
        static_cast<double>(total_mem),
        std::chrono::system_clock::now()
    };
    
    MetricData used_data{
        "ram_used",
        "KB",
        static_cast<double>(used_mem),
        std::chrono::system_clock::now()
    };
    
    result.push_back(usage_data);
    result.push_back(total_data);
    result.push_back(used_data);
    
    return result;
}

// Disk Usage Implementation
DiskUsageProvider::DiskUsageProvider(const MetricConfig& config)
    : MetricProvider(config) {}
    
std::vector<MetricData> DiskUsageProvider::collect() {
    std::vector<MetricData> result;
    
    // Get the mount point from config or use the default
    std::string mount_point = "/";
    if (config_.parameters.count("mount_point") > 0) {
        mount_point = config_.parameters.at("mount_point");
    }
    
    struct statvfs stat;
    if (statvfs(mount_point.c_str(), &stat) == 0) {
        double total = static_cast<double>(stat.f_blocks * stat.f_frsize);
        double available = static_cast<double>(stat.f_bavail * stat.f_frsize);
        double used = total - available;
        double usage_percent = 100.0 * used / total;
        
        MetricData usage_data{
            "disk_usage_percent",
            "%",
            usage_percent,
            std::chrono::system_clock::now()
        };
        
        MetricData total_data{
            "disk_total",
            "bytes",
            total,
            std::chrono::system_clock::now()
        };
        
        MetricData used_data{
            "disk_used",
            "bytes",
            used,
            std::chrono::system_clock::now()
        };
        
        result.push_back(usage_data);
        result.push_back(total_data);
        result.push_back(used_data);
    }
    
    return result;
}

// Factory Implementation
std::unique_ptr<MetricProvider> create_provider(const MetricConfig& config) {
    switch (config.type) {
        case MetricType::CPU_USAGE:
            return std::make_unique<CpuUsageProvider>(config);
        case MetricType::RAM_USAGE:
            return std::make_unique<RamUsageProvider>(config);
        case MetricType::DISK_USAGE:
            return std::make_unique<DiskUsageProvider>(config);
        default:
            spdlog::warn("Unsupported metric type for {}", config.name);
            return nullptr;
    }
}

} // namespace metrics