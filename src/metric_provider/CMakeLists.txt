set(LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/metric_provider.cpp")
set(LIBRARY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/metric_provider.hpp")
set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

add_library("MetricProvider" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("MetricProvider" PUBLIC ${LIBRARY_INCLUDES})
target_link_libraries(
    "MetricProvider"
    PRIVATE "ConfigParser"
            "MetricCollector"
            nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts)

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "MetricProvider"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        "MetricProvider"
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target("MetricProvider")
endif()
