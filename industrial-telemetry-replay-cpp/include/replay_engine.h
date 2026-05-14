#ifndef REPLAY_ENGINE_H
#define REPLAY_ENGINE_H

#include "telemetry_record.h"
#include "failure_detector.h"
#include <vector>
#include <chrono>
#include <functional>

/**
 * Configuration for replay session
 */
struct ReplayConfig {
    // Path to CSV data file
    std::string dataFilePath;

    // Replay parameters
    int delayMs = 100;  // Delay between records in milliseconds
    int limitRows = -1; // Max records to replay (-1 = all)
    bool verbose = true;
    bool showHeaders = true;
};

/**
 * Statistics collected during replay
 */
struct ReplayStats {
    int totalRecords = 0;
    int recordsReplayed = 0;
    int failuresDetected = 0;
    int warningsDetected = 0;
    int actualFailures = 0;
    std::chrono::milliseconds totalReplayTime{0};
};

/**
 * Replays machine telemetry data row-by-row as if it were live,
 * with configurable delays and real-time health monitoring.
 */
class ReplayEngine {
public:
    ReplayEngine(const ReplayConfig& config);

    /**
     * Start the replay session
     */
    void start();

    /**
     * Get replay statistics
     */
    const ReplayStats& getStats() const {
        return stats;
    }

    /**
     * Get the failure detector instance
     */
    const FailureDetector& getDetector() const {
        return detector;
    }

private:
    ReplayConfig config;
    ReplayStats stats;
    FailureDetector detector;
    std::vector<TelemetryRecord> records;

    /**
     * Print the telemetry record in a formatted way
     */
    void printRecord(const TelemetryRecord& record, int rowNumber, const HealthScore& health, const DetectionEvent* event);

    /**
     * Print replay session header
     */
    void printSessionHeader();

    /**
     * Print replay session footer with summary
     */
    void printSessionFooter();

    /**
     * Print a separator line
     */
    void printSeparator(int width = 150);

    /**
     * Format a double value to string with precision
     */
    std::string formatDouble(double value, int precision = 2) const;

    /**
     * Format an integer value to string
     */
    std::string formatInt(int value) const;

    /**
     * Get ANSI color code for health status
     */
    std::string getColorCode(HealthStatus status) const;

    /**
     * Reset ANSI color
     */
    std::string getColorReset() const;
};

#endif // REPLAY_ENGINE_H
