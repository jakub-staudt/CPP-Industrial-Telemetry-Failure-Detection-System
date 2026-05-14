#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "replay_engine.h"
#include "failure_detector.h"
#include <string>
#include <vector>

/**
 * Generates comprehensive session reports including statistics,
 * detected events, and failure analysis.
 */
class ReportGenerator {
public:
    /**
     * Generate a text-based session report
     * @param engine The replay engine that executed the session
     * @param outputPath Optional file path to write report (if empty, prints to stdout)
     */
    static void generateReport(const ReplayEngine& engine, const std::string& outputPath = "");

private:
    /**
     * Generate executive summary section
     */
    static std::string generateSummary(const ReplayEngine& engine);

    /**
     * Generate detection events section
     */
    static std::string generateDetectionSection(const ReplayEngine& engine);

    /**
     * Generate failure analysis section
     */
    static std::string generateFailureAnalysis(const ReplayEngine& engine);

    /**
     * Generate statistics tables
     */
    static std::string generateStatistics(const ReplayEngine& engine);

    /**
     * Format a markdown-style table
     */
    static std::string createTable(const std::vector<std::string>& headers,
                                   const std::vector<std::vector<std::string>>& rows);

    /**
     * Write content to file or stdout
     */
    static void output(const std::string& content, const std::string& filepath = "");
};

#endif // REPORT_GENERATOR_H
