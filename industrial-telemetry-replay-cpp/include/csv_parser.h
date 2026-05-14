#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <vector>
#include <string>
#include "telemetry_record.h"

/**
 * Parses the AI4I 2020 predictive maintenance CSV dataset.
 * Handles UTF-8 BOM and comma-separated values.
 */
class CsvParser {
public:
    /**
     * Load and parse a CSV file containing machine telemetry data.
     * @param filepath Path to the CSV file
     * @return Vector of TelemetryRecord objects
     * @throws std::runtime_error if file cannot be opened or parsing fails
     */
    static std::vector<TelemetryRecord> parseFile(const std::string& filepath);

private:
    /**
     * Split a CSV line into fields, handling quoted fields.
     * @param line The CSV line to split
     * @return Vector of field strings
     */
    static std::vector<std::string> splitCsvLine(const std::string& line);

    /**
     * Trim leading/trailing whitespace from a string.
     * @param s The string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& s);

    /**
     * Parse a single CSV line into a TelemetryRecord.
     * @param fields Vector of CSV fields
     * @return Parsed TelemetryRecord
     * @throws std::runtime_error if field count or format is invalid
     */
    static TelemetryRecord parseRecord(const std::vector<std::string>& fields);
};

#endif // CSV_PARSER_H
