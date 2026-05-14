#ifndef TELEMETRY_RECORD_H
#define TELEMETRY_RECORD_H

#include <string>
#include <iostream>

/**
 * Represents a single row of machine telemetry data from the AI4I 2020 dataset.
 * Contains both operational metrics and failure indicators.
 */
struct TelemetryRecord {
    // Unique identifier
    int uid;

    // Product metadata
    std::string productId;
    std::string type;

    // Temperature metrics (Kelvin)
    double airTemperatureK;
    double processTemperatureK;

    // Mechanical metrics
    int rotationalSpeedRpm;
    double torqueNm;
    int toolWearMin;

    // Failure indicators (target labels - NOT used for health score)
    bool machineFailure;
    bool twf;  // Tool Wear Failure
    bool hdf;  // Heat Dissipation Failure
    bool pwf;  // Power Failure
    bool osf;  // Overstrain Failure
    bool rnf;  // Random Nonfatal Failure

    /**
     * Human-readable string representation of the telemetry record
     */
    std::string toString() const;

    /**
     * Check if this record has any failure condition
     */
    bool hasAnyFailure() const;

    /**
     * Get failure type as a string (for reporting)
     */
    std::string getFailureType() const;
};

#endif // TELEMETRY_RECORD_H
