#include "csv_parser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>

std::vector<TelemetryRecord> CsvParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open CSV file: " + filepath);
    }

    std::vector<TelemetryRecord> records;
    std::string line;

    // Skip header line (also removes UTF-8 BOM if present)
    if (!std::getline(file, line)) {
        throw std::runtime_error("CSV file is empty");
    }

    // Remove UTF-8 BOM if present
    if (!line.empty() && (unsigned char)line[0] == 0xEF) {
        line = line.substr(3);
    }

    int lineNum = 1;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;

        try {
            auto fields = splitCsvLine(line);
            records.push_back(parseRecord(fields));
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing line " + std::to_string(lineNum) + ": " + e.what());
        }
    }

    return records;
}

std::vector<std::string> CsvParser::splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(trim(field));
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(trim(field));

    return fields;
}

std::string CsvParser::trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

TelemetryRecord CsvParser::parseRecord(const std::vector<std::string>& fields) {
    if (fields.size() != 13) {
        throw std::runtime_error("Expected 13 fields, got " + std::to_string(fields.size()));
    }

    TelemetryRecord record;

    try {
        record.uid = std::stoi(fields[0]);
        record.productId = fields[1];
        record.type = fields[2];
        record.airTemperatureK = std::stod(fields[3]);
        record.processTemperatureK = std::stod(fields[4]);
        record.rotationalSpeedRpm = std::stoi(fields[5]);
        record.torqueNm = std::stod(fields[6]);
        record.toolWearMin = std::stoi(fields[7]);
        record.machineFailure = std::stoi(fields[8]) != 0;
        record.twf = std::stoi(fields[9]) != 0;
        record.hdf = std::stoi(fields[10]) != 0;
        record.pwf = std::stoi(fields[11]) != 0;
        record.osf = std::stoi(fields[12]) != 0;
        record.rnf = fields.size() > 13 ? std::stoi(fields[13]) != 0 : false;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Field parsing error: ") + e.what());
    }

    return record;
}
