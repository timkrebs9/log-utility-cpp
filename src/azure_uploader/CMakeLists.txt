set(LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/azure_uploader.cpp")
set(LIBRARY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/azure_uploader.hpp")
set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

add_library("AzureUploader" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("AzureUploader" PUBLIC ${LIBRARY_INCLUDES})
target_link_libraries(
    "AzureUploader"
    PRIVATE nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts)

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "AzureUploader"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        "AzureUploader"
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target("AzureUploader")
endif()
