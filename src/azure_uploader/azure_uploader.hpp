#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace metrics {

class AzureUploader {
public:
    AzureUploader(const std::string& connection_string, const std::string& container_name);
    
    // Upload metrics data to Azure Blob Storage
    bool upload_metrics(const nlohmann::json& metrics_data);
    
private:
    std::string connection_string_;
    std::string container_name_;
};

} // namespace metrics