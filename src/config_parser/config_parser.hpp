#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace metrics
{

enum class MetricType
{
    CPU_USAGE,
    CPU_TEMPERATURE,
    RAM_USAGE,
    DISK_USAGE,
    NETWORK_BANDWIDTH,
    PROCESS_COUNT,
    CUSTOM
};

struct MetricConfig
{
    MetricType type;
    std::string name;
    bool enabled;
    std::unordered_map<std::string, std::string> parameters;

    MetricConfig()
        : type(MetricType::CPU_USAGE), name(""), enabled(false), parameters()
    {
    }
};

class ConfigParser
{
public:
    explicit ConfigParser(const std::filesystem::path &config_path);

    std::vector<MetricConfig> get_enabled_metrics() const;
    int get_collection_interval_seconds() const;
    std::string get_azure_connection_string() const;
    std::string get_azure_container_name() const;

private:
    std::vector<MetricConfig> metrics_;
    int collection_interval_seconds_ = 10;
    std::string azure_connection_string_;
    std::string azure_container_name_;

    void parse_config(const nlohmann::json &config);
    MetricType string_to_metric_type(const std::string &type_str);
};

} // namespace metrics
