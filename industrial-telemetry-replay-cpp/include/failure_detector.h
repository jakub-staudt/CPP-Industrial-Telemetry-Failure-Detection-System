#ifndef FAILURE_DETECTOR_H
#define FAILURE_DETECTOR_H

#include "telemetry_record.h"
#include "health_calculator.h"
#include <vector>

/**
 * Represents a detection event (warning or critical condition)
 */
struct DetectionEvent {
    int rowNumber;
    const TelemetryRecord* record;
    HealthScore health;
    std::string eventType;  // "WARNING", "CRITICAL", "FAILURE_CONFIRMED"
    std::string description;
    bool wasAccurate;  // True if actual failure occurred at or shortly after this event

    std::string toString() const;
};

/**
 * Detects warning and critical machine health conditions.
 * Maintains state across records to identify trends and predict failures.
 */
class FailureDetector {
public:
    FailureDetector();

    /**
     * Process a new telemetry record for potential failure conditions.
     * @param record The telemetry record to analyze
     * @param rowNumber Current row number (1-indexed)
     * @return Pointer to DetectionEvent if condition detected, nullptr otherwise
     */
    DetectionEvent* processRecord(const TelemetryRecord& record, int rowNumber);

    /**
     * Get all detected events so far
     */
    const std::vector<DetectionEvent>& getDetectionEvents() const {
        return detectionEvents;
    }

    /**
     * Validate detection accuracy against known failures
     */
    void validateDetections();

    /**
     * Get detection statistics
     */
    struct DetectionStats {
        int totalWarnings;
        int totalCritical;
        int correctWarnings;
        int missedFailures;
        double accuracyRate;
    };

    DetectionStats getStats() const;

private:
    std::vector<DetectionEvent> detectionEvents;

    // State tracking
    int previousHealthStatus = 0;  // 0 = HEALTHY, 1 = WARNING, 2 = CRITICAL
    int warningConsecutiveCount = 0;
    double maxTemperatureSeen = 0.0;
    int maxWearSeen = 0;

    /**
     * Check for critical failure conditions
     */
    bool isCriticalFailure(const HealthScore& health, const TelemetryRecord& record);

    /**
     * Check for warning conditions
     */
    bool isWarningCondition(const HealthScore& health, const TelemetryRecord& record);

    /**
     * Analyze pattern changes in telemetry
     */
    std::string analyzeTrends(const TelemetryRecord& record, const HealthScore& health);
};

#endif // FAILURE_DETECTOR_H
