set(LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/metric_collector.cpp")
set(LIBRARY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/metric_collector.hpp")
set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

add_library("MetricCollector" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("MetricCollector" PUBLIC ${LIBRARY_INCLUDES})
target_link_libraries(
    "MetricCollector"
    PRIVATE "ConfigParser"
            "MetricProvider"
            nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts)

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "MetricCollector"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        "MetricCollector"
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target("MetricCollector")
endif()
