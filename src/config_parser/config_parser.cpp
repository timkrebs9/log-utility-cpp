#include "config_parser.hpp"
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

namespace metrics
{

ConfigParser::ConfigParser(const std::filesystem::path &config_path)
    : metrics_(), azure_connection_string_(), azure_container_name_()
{
    try
    {
        std::ifstream config_file(config_path);
        if (!config_file.is_open())
        {
            throw std::runtime_error("Failed to open config file: " +
                                     config_path.string());
        }

        nlohmann::json config = nlohmann::json::parse(config_file);
        parse_config(config);
    }
    catch (const std::exception &ex)
    {
        spdlog::error("Error parsing configuration: {}", ex.what());
        throw;
    }
}

void ConfigParser::parse_config(const nlohmann::json &config)
{
    // Parse collection interval
    if (config.contains("collection_interval_seconds"))
    {
        collection_interval_seconds_ =
            config["collection_interval_seconds"].get<int>();
    }

    // Parse Azure Settings
    if (config.contains("azure"))
    {
        const auto &azure = config["azure"];

        if (azure.contains("storage_account_url"))
        {
            azure_connection_string_ =
                azure["storage_account_url"].get<std::string>();
        }
        if (azure.contains("storage_account_name"))
        {
            azure_container_name_ =
                azure["storage_account_name"].get<std::string>();
        }
        if (azure.contains("storage_account_key"))
        {
            azure_container_name_ =
                azure["storage_account_key"].get<std::string>();
        }
    }

    // Parse metrics
    if (config.contains("metrics") && config["metrics"].is_array())
    {
        for (const auto &metric : config["metrics"])
        {
            MetricConfig mc;
            mc.name = metric["name"].get<std::string>();
            mc.enabled = metric.value("enabled", false);
            mc.type = string_to_metric_type(metric["type"].get<std::string>());

            // Parse optional parameters
            if (metric.contains("parameters") &&
                metric["parameters"].is_object())
            {
                for (auto &[key, value] : metric["parameters"].items())
                {
                    mc.parameters[key] = value.get<std::string>();
                }
            }

            metrics_.push_back(mc);
        }
    }
}

MetricType ConfigParser::string_to_metric_type(const std::string &type_str)
{
    if (type_str == "cpu_usage")
        return MetricType::CPU_USAGE;
    if (type_str == "cpu_temperature")
        return MetricType::CPU_TEMPERATURE;
    if (type_str == "ram_usage")
        return MetricType::RAM_USAGE;
    if (type_str == "disk_usage")
        return MetricType::DISK_USAGE;
    if (type_str == "network_bandwidth")
        return MetricType::NETWORK_BANDWIDTH;
    if (type_str == "process_count")
        return MetricType::PROCESS_COUNT;
    return MetricType::CUSTOM;
}

std::vector<MetricConfig> ConfigParser::get_enabled_metrics() const
{
    std::vector<MetricConfig> enabled;
    for (const auto &metric : metrics_)
    {
        if (metric.enabled)
        {
            enabled.push_back(metric);
        }
    }
    return enabled;
}

int ConfigParser::get_collection_interval_seconds() const
{
    return collection_interval_seconds_;
}

std::string ConfigParser::get_azure_connection_string() const
{
    return azure_connection_string_;
}

std::string ConfigParser::get_azure_container_name() const
{
    return azure_container_name_;
}

} // namespace metrics
