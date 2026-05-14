#include "telemetry_record.h"
#include <sstream>
#include <iomanip>

std::string TelemetryRecord::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "UID: " << uid << " | "
        << "Product: " << productId << " | "
        << "Type: " << type << " | "
        << "AirTemp: " << airTemperatureK << "K | "
        << "ProcTemp: " << processTemperatureK << "K | "
        << "Speed: " << rotationalSpeedRpm << "rpm | "
        << "Torque: " << torqueNm << "Nm | "
        << "Wear: " << toolWearMin << "min";
    return oss.str();
}

bool TelemetryRecord::hasAnyFailure() const {
    return machineFailure || twf || hdf || pwf || osf || rnf;
}

std::string TelemetryRecord::getFailureType() const {
    if (!hasAnyFailure()) return "NONE";

    std::string failures;
    if (twf) failures += "TWF ";
    if (hdf) failures += "HDF ";
    if (pwf) failures += "PWF ";
    if (osf) failures += "OSF ";
    if (rnf) failures += "RNF ";

    return failures.empty() ? "UNKNOWN" : failures;
}
