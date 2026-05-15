#include "health_calculator.h"
#include <cmath>
#include <algorithm>

std::string HealthScore::getStatusString() const {
    switch (status) {
        case HealthStatus::HEALTHY:
            return "HEALTHY";
        case HealthStatus::WARNING:
            return "WARNING";
        case HealthStatus::CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

HealthScore HealthCalculator::calculateHealth(const TelemetryRecord& record) {
    HealthScore result;

    // Calculate component scores
    result.temperatureScore = calculateTemperatureScore(record.airTemperatureK, record.processTemperatureK);
    result.speedScore = calculateSpeedScore(record.rotationalSpeedRpm);
    result.torqueScore = calculateTorqueScore(record.torqueNm);
    result.wearScore = calculateWearScore(record.toolWearMin);

    // Weighted average: wear is most critical, temperature and torque next, speed last
    result.score = (result.wearScore * 0.35) +
                   (result.temperatureScore * 0.25) +
                   (result.torqueScore * 0.25) +
                   (result.speedScore * 0.15);

    // Determine status
    if (result.score < CRITICAL_THRESHOLD) {
        result.status = HealthStatus::CRITICAL;
    } else if (result.score < WARNING_THRESHOLD) {
        result.status = HealthStatus::WARNING;
    } else {
        result.status = HealthStatus::HEALTHY;
    }

    return result;
}

double HealthCalculator::calculateTemperatureScore(double airTemp, double processTemp) {
    // Normal range: air ~298K, process ~309K
    // Deviations indicate stress
    double airDeviation = std::abs(airTemp - NORMAL_AIR_TEMP);
    double procDeviation = std::abs(processTemp - NORMAL_PROCESS_TEMP);

    // Penalize large deviations
    double tempPenalty = (airDeviation * 2.0) + (procDeviation * 1.5);

    // Score: 100 - penalty, clamped to [0, 100]
    double score = 100.0 - tempPenalty;
    return std::clamp(score, 0.0, 100.0);
}

double HealthCalculator::calculateSpeedScore(int speed) {
    // Normal speed ~1500 rpm
    // Extreme speeds (very high or very low) indicate issues
    double speedDeviation = std::abs(speed - NORMAL_SPEED);

    // Allow some variance; penalize larger deviations
    if (speedDeviation < 200) {
        return 100.0;  // Good tolerance for normal variance
    } else if (speedDeviation < 500) {
        return 90.0 - ((speedDeviation - 200) * 0.02);
    } else {
        return 70.0 - ((speedDeviation - 500) * 0.01);
    }
}

double HealthCalculator::calculateTorqueScore(double torque) {
    // Normal torque ~40 Nm
    // High torque indicates stress; low torque may indicate bearing issues
    double torqueDeviation = std::abs(torque - NORMAL_TORQUE);

    if (torqueDeviation < 5.0) {
        return 100.0;
    } else if (torqueDeviation < 15.0) {
        return 95.0 - ((torqueDeviation - 5.0) * 0.5);
    } else if (torque > 60.0) {
        // High torque is more concerning than low
        return 70.0 - ((torque - 60.0) * 0.5);
    } else {
        return 75.0;
    }
}

double HealthCalculator::calculateWearScore(int wearMin) {
    // Tool wear accumulates linearly
    // Critical threshold around 250+ minutes
    // Warning around 150-250 minutes
    if (wearMin < 50) {
        return 100.0;
    } else if (wearMin < 150) {
        return 100.0 - ((wearMin - 50) * 0.5);
    } else if (wearMin < 250) {
        return 75.0 - ((wearMin - 150) * 0.5);
    } else {
        return 25.0 - ((wearMin - 250) * 0.1);
    }
}
