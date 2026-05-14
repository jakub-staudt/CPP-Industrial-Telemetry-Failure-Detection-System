#include "report_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

void ReportGenerator::generateReport(const ReplayEngine& engine, const std::string& outputPath) {
    std::ostringstream report;

    report << "\n";
    report << "═══════════════════════════════════════════════════════════\n";
    report << "   Industrial Telemetry Replay - Session Report\n";
    report << "═══════════════════════════════════════════════════════════\n";
    report << "\n";

    report << generateSummary(engine);
    report << "\n";
    report << generateDetectionSection(engine);
    report << "\n";
    report << generateFailureAnalysis(engine);
    report << "\n";
    report << generateStatistics(engine);

    output(report.str(), outputPath);
}

std::string ReportGenerator::generateSummary(const ReplayEngine& engine) {
    std::ostringstream os;
    const auto& stats = engine.getStats();

    os << "EXECUTIVE SUMMARY\n";
    os << "─────────────────────────────────────────────────────────────\n";
    os << "Records Processed:     " << stats.recordsReplayed << " / " << stats.totalRecords << "\n";
    os << "Actual Failures:       " << stats.actualFailures << "\n";
    os << "Critical Detections:   " << stats.failuresDetected << "\n";
    os << "Warning Detections:    " << stats.warningsDetected << "\n";
    os << "Total Session Time:    " << stats.totalReplayTime.count() << "ms\n";
    os << "\n";

    return os.str();
}

std::string ReportGenerator::generateDetectionSection(const ReplayEngine& engine) {
    std::ostringstream os;
    const auto& events = engine.getDetector().getDetectionEvents();

    os << "DETECTION EVENTS\n";
    os << "─────────────────────────────────────────────────────────────\n";

    if (events.empty()) {
        os << "No events detected during session.\n";
    } else {
        for (const auto& event : events) {
            os << "\n[Row " << event.rowNumber << "] " << event.eventType << "\n";
            os << "  Health Score: " << std::fixed << std::setprecision(2) << event.health.score << "\n";
            os << "  Status: " << event.health.getStatusString() << "\n";
            os << "  Description: " << event.description << "\n";
            if (event.wasAccurate) {
                os << "  Result: ✓ Accurate (actual failure confirmed)\n";
            }
        }
    }

    os << "\n";
    return os.str();
}

std::string ReportGenerator::generateFailureAnalysis(const ReplayEngine& engine) {
    std::ostringstream os;
    const auto& detector = engine.getDetector();
    auto stats = detector.getStats();

    os << "FAILURE ANALYSIS\n";
    os << "─────────────────────────────────────────────────────────────\n";
    os << "Total Critical Events:  " << stats.totalCritical << "\n";
    os << "Total Warnings:         " << stats.totalWarnings << "\n";
    os << "Correct Detections:     " << stats.correctWarnings << "\n";
    os << "Detection Accuracy:     " << std::fixed << std::setprecision(1) << (stats.accuracyRate * 100.0) << "%\n";
    os << "\n";

    return os.str();
}

std::string ReportGenerator::generateStatistics(const ReplayEngine& engine) {
    std::ostringstream os;
    const auto& stats = engine.getStats();

    os << "PERFORMANCE METRICS\n";
    os << "─────────────────────────────────────────────────────────────\n";
    os << "Average Record Processing Time: " << std::fixed << std::setprecision(3)
       << (stats.totalReplayTime.count() / (double)stats.recordsReplayed) << "ms\n";
    os << "Total Session Duration:         " << stats.totalReplayTime.count() << "ms\n";
    os << "Throughput:                     " << std::fixed << std::setprecision(0)
       << (stats.recordsReplayed * 1000.0 / stats.totalReplayTime.count()) << " records/sec\n";
    os << "\n";

    return os.str();
}

std::string ReportGenerator::createTable(const std::vector<std::string>& headers,
                                        const std::vector<std::vector<std::string>>& rows) {
    std::ostringstream os;

    // Print headers
    for (size_t i = 0; i < headers.size(); ++i) {
        os << "| " << std::setw(15) << std::left << headers[i] << " ";
    }
    os << "|\n";

    // Print separator
    os << "|";
    for (size_t i = 0; i < headers.size(); ++i) {
        os << std::string(17, '-') << "|";
    }
    os << "\n";

    // Print rows
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            os << "| " << std::setw(15) << std::left << row[i] << " ";
        }
        os << "|\n";
    }

    return os.str();
}

void ReportGenerator::output(const std::string& content, const std::string& filepath) {
    if (filepath.empty()) {
        // Print to stdout
        std::cout << content;
    } else {
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filepath);
        }
        file << content;
        std::cout << "Report written to: " << filepath << "\n";
    }
}
