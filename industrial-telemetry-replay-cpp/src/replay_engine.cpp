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

    printSessionHeader();

    int limit = config.limitRows > 0 ? std::min(config.limitRows, (int)records.size()) : records.size();
    stats.totalRecords = records.size();

    for (int i = 0; i < limit; ++i) {
        const auto& record = records[i];
        HealthScore health = HealthCalculator::calculateHealth(record);

        // Process with detector
        DetectionEvent* event = detector.processRecord(record, i + 1);

        // Print the record
        printRecord(record, i + 1, health, event);

        // Update statistics
        stats.recordsReplayed++;
        if (record.machineFailure) {
            stats.actualFailures++;
        }
        if (event) {
            if (event->eventType == "CRITICAL" || event->eventType == "FAILURE_CONFIRMED") {
                stats.failuresDetected++;
            } else if (event->eventType == "WARNING") {
                stats.warningsDetected++;
            }
        }

        // Sleep unless this is the last record
        if (i < limit - 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.delayMs));
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    stats.totalReplayTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    // Validate detections
    detector.validateDetections();

    printSessionFooter();
}

void ReplayEngine::printSessionHeader() {
    if (!config.verbose) return;

    printSeparator();
    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║  Industrial Telemetry Replay & Failure Detection System   ║\n";
    std::cout << "  ║  AI4I 2020 Predictive Maintenance Dataset                 ║\n";
    std::cout << "  ╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    std::cout << "  Session Configuration:\n";
    std::cout << "  ├─ Data Source: " << config.dataFilePath << "\n";
    std::cout << "  ├─ Replay Delay: " << config.delayMs << "ms\n";
    std::cout << "  ├─ Row Limit: " << (config.limitRows > 0 ? std::to_string(config.limitRows) : "ALL") << "\n";
    std::cout << "  └─ Total Records Available: " << stats.totalRecords << "\n";
    std::cout << "\n";

    printSeparator();
    std::cout << "\n";

    if (config.showHeaders) {
        std::cout << std::setw(6) << "Row"
                  << std::setw(12) << "Health"
                  << std::setw(10) << "Score"
                  << std::setw(15) << "AirTemp(K)"
                  << std::setw(16) << "ProcTemp(K)"
                  << std::setw(12) << "Speed(rpm)"
                  << std::setw(12) << "Torque(Nm)"
                  << std::setw(12) << "Wear(min)"
                  << std::setw(20) << "Event"
                  << "\n";
        printSeparator();
    }
}

void ReplayEngine::printRecord(const TelemetryRecord& record, int rowNumber, const HealthScore& health, const DetectionEvent* event) {
    if (!config.verbose) return;

    std::string colorCode = getColorCode(health.status);
    std::string colorReset = getColorReset();

    std::cout << colorCode
              << std::setw(6) << rowNumber
              << std::setw(12) << health.getStatusString()
              << std::setw(10) << std::fixed << std::setprecision(1) << health.score
              << std::setw(15) << std::fixed << std::setprecision(1) << record.airTemperatureK
              << std::setw(16) << std::fixed << std::setprecision(1) << record.processTemperatureK
              << std::setw(12) << record.rotationalSpeedRpm
              << std::setw(12) << std::fixed << std::setprecision(1) << record.torqueNm
              << std::setw(12) << record.toolWearMin;

    if (event) {
        std::cout << std::setw(20) << event->eventType;
    } else if (record.hasAnyFailure()) {
        std::cout << std::setw(20) << "ACTUAL_FAILURE";
    } else {
        std::cout << std::setw(20) << "-";
    }

    std::cout << colorReset << "\n";

    // Print event description on next line if present
    if (event && !event->description.empty()) {
        std::cout << colorCode << "  ⚠ " << event->description << colorReset << "\n";
    }
    if (record.hasAnyFailure() && !event) {
        std::cout << "  💥 ACTUAL FAILURE: " << record.getFailureType() << "\n";
    }
}

void ReplayEngine::printSessionFooter() {
    if (!config.verbose) return;

    std::cout << "\n";
    printSeparator();
    std::cout << "\n";
    std::cout << "  Session Summary:\n";
    std::cout << "  ├─ Records Replayed: " << stats.recordsReplayed << "\n";
    std::cout << "  ├─ Actual Failures Detected: " << stats.actualFailures << "\n";
    std::cout << "  ├─ Critical Warnings: " << stats.failuresDetected << "\n";
    std::cout << "  ├─ Warnings: " << stats.warningsDetected << "\n";
    std::cout << "  ├─ Total Replay Time: " << stats.totalReplayTime.count() << "ms\n";

    auto detectorStats = detector.getStats();
    std::cout << "  ├─ Detection Accuracy: " << std::fixed << std::setprecision(1)
              << (detectorStats.accuracyRate * 100.0) << "%\n";
    std::cout << "  └─ Avg Record Processing: " << (stats.totalReplayTime.count() / (double)stats.recordsReplayed)
              << "ms\n";
    std::cout << "\n";
    printSeparator();
    std::cout << "\n";
}

void ReplayEngine::printSeparator(int width) {
    std::cout << std::string(width, '─') << "\n";
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
