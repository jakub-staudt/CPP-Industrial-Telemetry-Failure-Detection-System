#include "failure_detector.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string DetectionEvent::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "[Row " << rowNumber << "] " << eventType << " - "
        << "Score: " << health.score << " | "
        << "Status: " << health.getStatusString() << " | "
        << description;
    return oss.str();
}

FailureDetector::FailureDetector() : previousHealthStatus(0) {}

DetectionEvent* FailureDetector::processRecord(const TelemetryRecord& record, int rowNumber) {
    HealthScore health = HealthCalculator::calculateHealth(record);

    // Update state
    maxTemperatureSeen = std::max(maxTemperatureSeen, record.processTemperatureK);
    maxWearSeen = std::max(maxWearSeen, record.toolWearMin);

    DetectionEvent* event = nullptr;

    // Check for critical failure
    if (isCriticalFailure(health, record)) {
        detectionEvents.emplace_back();
        DetectionEvent& e = detectionEvents.back();
        e.rowNumber = rowNumber;
        e.record = &record;
        e.health = health;
        e.eventType = "CRITICAL";
        e.description = analyzeTrends(record, health);
        e.wasAccurate = false;  // Will be validated later
        event = &e;
        previousHealthStatus = 2;
        warningConsecutiveCount = 0;
    }
    // Check for warning condition
    else if (isWarningCondition(health, record)) {
        warningConsecutiveCount++;
        if (warningConsecutiveCount >= 1) {  // Trigger on first warning
            detectionEvents.emplace_back();
            DetectionEvent& e = detectionEvents.back();
            e.rowNumber = rowNumber;
            e.record = &record;
            e.health = health;
            e.eventType = "WARNING";
            e.description = analyzeTrends(record, health);
            e.wasAccurate = false;
            event = &e;
            previousHealthStatus = 1;
        }
    } else {
        previousHealthStatus = 0;
        warningConsecutiveCount = 0;
    }

    // Check if this is an actual failure confirmation
    if (event && record.hasAnyFailure()) {
        event->eventType = "FAILURE_CONFIRMED";
        event->wasAccurate = true;
    }

    return event;
}

void FailureDetector::validateDetections() {
    // Check each detection against actual failures
    for (auto& event : detectionEvents) {
        if (event.record && event.record->hasAnyFailure()) {
            event.wasAccurate = true;
        }
    }
}

FailureDetector::DetectionStats FailureDetector::getStats() const {
    DetectionStats stats{};
    stats.totalWarnings = 0;
    stats.totalCritical = 0;
    stats.correctWarnings = 0;
    stats.missedFailures = 0;

    for (const auto& event : detectionEvents) {
        if (event.eventType == "CRITICAL" || event.eventType == "FAILURE_CONFIRMED") {
            stats.totalCritical++;
            if (event.wasAccurate || event.record->hasAnyFailure()) {
                stats.correctWarnings++;
            }
        } else if (event.eventType == "WARNING") {
            stats.totalWarnings++;
        }
    }

    if (stats.totalCritical + stats.totalWarnings > 0) {
        stats.accuracyRate = (double)stats.correctWarnings / (stats.totalCritical + stats.totalWarnings);
    } else {
        stats.accuracyRate = 0.0;
    }

    return stats;
}

bool FailureDetector::isCriticalFailure(const HealthScore& health, const TelemetryRecord& record) {
    // Critical if:
    // 1. Health score below critical threshold
    if (health.status == HealthStatus::CRITICAL) {
        return true;
    }

    // 2. Extreme temperature spike
    if (record.processTemperatureK > 315.0) {
        return true;
    }

    // 3. Excessive tool wear
    if (record.toolWearMin > 240) {
        return true;
    }

    // 4. Extreme torque (>70 Nm)
    if (record.torqueNm > 70.0) {
        return true;
    }

    return false;
}

bool FailureDetector::isWarningCondition(const HealthScore& health, const TelemetryRecord& record) {
    // Warning if:
    // 1. Health score in warning range
    if (health.status == HealthStatus::WARNING) {
        return true;
    }

    // 2. Temperature approaching critical
    if (record.processTemperatureK > 312.0) {
        return true;
    }

    // 3. Tool wear approaching limits
    if (record.toolWearMin > 180) {
        return true;
    }

    // 4. Torque consistently elevated
    if (record.torqueNm > 55.0) {
        return true;
    }

    return false;
}

std::string FailureDetector::analyzeTrends(const TelemetryRecord& record, const HealthScore& health) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);

    std::string reason;

    // Identify the primary factor
    if (health.wearScore < 50) {
        oss << "Critical tool wear (" << record.toolWearMin << "min)";
        reason = oss.str();
    } else if (health.temperatureScore < 60) {
        oss << "Abnormal temperature pattern (Air:" << record.airTemperatureK
            << "K, Process:" << record.processTemperatureK << "K)";
        reason = oss.str();
    } else if (health.torqueScore < 60) {
        oss << "High torque stress (" << record.torqueNm << "Nm)";
        reason = oss.str();
    } else if (health.speedScore < 70) {
        oss << "Rotational speed anomaly (" << record.rotationalSpeedRpm << "rpm)";
        reason = oss.str();
    } else {
        reason = "Multi-factor degradation detected";
    }

    return reason;
}
