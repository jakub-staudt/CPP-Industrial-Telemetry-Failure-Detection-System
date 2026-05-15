#include "replay_engine.h"
#include "csv_parser.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

ReplayEngine::ReplayEngine(const ReplayConfig& config)
    : config(config) {
    try {
        records = CsvParser::parseFile(config.dataFilePath);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to load CSV: ") + e.what());
    }
}

void ReplayEngine::start() {
    auto startTime = std::chrono::high_resolution_clock::now();
 
    int limit = config.limitRows > 0 ? std::min(config.limitRows, (int)records.size()) : records.size();
    stats.totalRecords = records.size();
 
    for (int i = 0; i < limit; ++i) {
        const auto& record = records[i];
        HealthCalculator::calculateHealth(record);
 
        // Process with detector
        detector.processRecord(record, i + 1);
 
        // Update statistics
        stats.recordsReplayed++;
        if (record.machineFailure) {
            stats.actualFailures++;
        }
    }
 
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.totalReplayTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
 
    // Validate detections
    detector.validateDetections();
}

void ReplayEngine::printSessionHeader() {}
void ReplayEngine::printRecord(const TelemetryRecord&, int, const HealthScore&, const DetectionEvent*) {}
void ReplayEngine::printSessionFooter() {}

void ReplayEngine::printSeparator(int width) {
    for (int i = 0; i < width; ++i) std::cout << "─";
    std::cout << "\n";
}

std::string ReplayEngine::getColorCode(HealthStatus status) const {
    switch (status) {
        case HealthStatus::HEALTHY:
            return "\033[92m";  // Green
        case HealthStatus::WARNING:
            return "\033[93m";  // Yellow
        case HealthStatus::CRITICAL:
            return "\033[91m";  // Red
        default:
            return "";
    }
}

std::string ReplayEngine::getColorReset() const {
    return "\033[0m";
}
