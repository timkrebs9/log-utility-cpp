set(LIBRARY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/config_parser.cpp")
set(LIBRARY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/config_parser.hpp")
set(LIBRARY_INCLUDES "./" "${CMAKE_BINARY_DIR}/configured_files/include")

add_library("ConfigParser" STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories("ConfigParser" PUBLIC ${LIBRARY_INCLUDES})
target_link_libraries(
    "ConfigParser"
    PRIVATE nlohmann_json::nlohmann_json
            fmt::fmt
            spdlog::spdlog
            cxxopts::cxxopts)

if(${ENABLE_WARNINGS})
    target_set_warnings(
        TARGET
        "ConfigParser"
        ENABLE
        ${ENABLE_WARNINGS}
        AS_ERRORS
        ${ENABLE_WARNINGS_AS_ERRORS})
endif()

if(${ENABLE_LTO})
    target_enable_lto(
        TARGET
        "ConfigParser"
        ENABLE
        ON)
endif()

if(${ENABLE_CLANG_TIDY})
    add_clang_tidy_to_target("ConfigParser")
endif()
