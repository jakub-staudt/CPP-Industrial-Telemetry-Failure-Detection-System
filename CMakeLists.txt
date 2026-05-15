cmake_minimum_required(VERSION 3.16)
project(industrial_telemetry_replay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Main executable
add_executable(industrial_monitor
    src/main.cpp
    src/csv_parser.cpp
    src/telemetry_record.cpp
    src/replay_engine.cpp
    src/health_calculator.cpp
    src/failure_detector.cpp
    src/report_generator.cpp
)

target_include_directories(industrial_monitor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Optional: if using system libraries
find_package(Threads REQUIRED)
target_link_libraries(industrial_monitor PRIVATE Threads::Threads)
