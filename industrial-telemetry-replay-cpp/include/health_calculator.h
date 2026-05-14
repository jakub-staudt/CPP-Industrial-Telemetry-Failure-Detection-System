#ifndef HEALTH_CALCULATOR_H
#define HEALTH_CALCULATOR_H

#include "telemetry_record.h"

/**
 * Enum representing machine health status levels
 */
enum class HealthStatus {
    HEALTHY = 0,
    WARNING = 1,
    CRITICAL = 2
};

/**
 * Result of health calculation for a telemetry record
 */
struct HealthScore {
    // Overall health score (0.0 = critical, 100.0 = healthy)
    double score;

    // Status level (HEALTHY, WARNING, or CRITICAL)
    HealthStatus status;

    // Individual component scores for diagnosis
    double temperatureScore;
    double speedScore;
    double torqueScore;
    double wearScore;

    // Status transitions
    std::string getStatusString() const;
};

/**
 * Calculates machine health based on telemetry metrics.
 * Uses a multi-factor weighted algorithm that combines:
 * - Temperature patterns (air and process temps)
 * - Rotational speed anomalies
 * - Torque stress levels
 * - Tool wear accumulation
 */
class HealthCalculator {
public:
    /**
     * Calculate health score for a telemetry record.
     * @param record The telemetry record to evaluate
     * @return HealthScore with overall score and component breakdown
     */
    static HealthScore calculateHealth(const TelemetryRecord& record);

private:
    // Thresholds for health status transitions
    static constexpr double CRITICAL_THRESHOLD = 40.0;
    static constexpr double WARNING_THRESHOLD = 70.0;

    // Baseline/reference values for anomaly detection
    static constexpr double NORMAL_AIR_TEMP = 298.0;
    static constexpr double NORMAL_PROCESS_TEMP = 309.0;
    static constexpr double NORMAL_SPEED = 1500;
    static constexpr double NORMAL_TORQUE = 40.0;

    /**
     * Evaluate temperature deviation from normal operating range.
     * @param airTemp Air temperature in Kelvin
     * @param processTemp Process temperature in Kelvin
     * @return Score 0-100 (100 = healthy)
     */
    static double calculateTemperatureScore(double airTemp, double processTemp);

    /**
     * Evaluate rotational speed for anomalies.
     * @param speed Rotational speed in RPM
     * @return Score 0-100 (100 = healthy)
     */
    static double calculateSpeedScore(int speed);

    /**
     * Evaluate torque stress levels.
     * @param torque Torque in Nm
     * @return Score 0-100 (100 = healthy)
     */
    static double calculateTorqueScore(double torque);

    /**
     * Evaluate tool wear degradation.
     * @param wearMin Tool wear in minutes
     * @return Score 0-100 (100 = healthy)
     */
    static double calculateWearScore(int wearMin);
};

#endif // HEALTH_CALCULATOR_H
