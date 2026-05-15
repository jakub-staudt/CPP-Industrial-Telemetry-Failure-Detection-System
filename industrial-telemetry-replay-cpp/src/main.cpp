#include "replay_engine.h"
#include "report_generator.h"
#include <iostream>
#include <string>
#include <cstring>

/**
 * Print usage information
 */
void printUsage(const char* programName) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Industrial Telemetry Replay & Failure Detection System   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "USAGE:\n";
    std::cout << "  " << programName << " [OPTIONS]\n";
    std::cout << "\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  --input <file>         Input CSV file (default: data/equipment_telemetry.csv)\n";
    std::cout << "  --output <file>        Output CSV file (default: output/results.csv)\n";
    std::cout << "  --limit <n>            Maximum number of rows to process (default: all)\n";
    std::cout << "  --report <file>        Write detailed report to file (optional)\n";
    std::cout << "  --help                 Show this help message\n";
    std::cout << "\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  # Fast computation with default paths\n";
    std::cout << "  " << programName << "\n";
    std::cout << "\n";
    std::cout << "  # Process first 1000 rows\n";
    std::cout << "  " << programName << " --limit 1000\n";
    std::cout << "\n";
    std::cout << "  # Custom input and output paths\n";
    std::cout << "  " << programName << " --input custom.csv --output custom_results.csv\n";
    std::cout << "\n";
}

/**
 * Parse command-line arguments
 */
bool parseArguments(int argc, char* argv[], ReplayConfig& config, std::string& reportPath, std::string& csvPath, std::string& inputPath) {
    // Set defaults
    inputPath = "data/equipment_telemetry.csv";
    csvPath = "output/results.csv";

    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return false;
        }
    }
 
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
 
        if (arg == "--input" && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            csvPath = argv[++i];
        } else if (arg == "--limit" && i + 1 < argc) {
            config.limitRows = std::stoi(argv[++i]);
        } else if (arg == "--report" && i + 1 < argc) {
            reportPath = argv[++i];
        } else if (arg == "--quiet") {
            config.verbose = false;
        } else if (arg == "--no-headers") {
            config.showHeaders = false;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            std::cerr << "Use --help for usage information\n";
            return false;
        }
    }
 
    return true;
}

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
    try {
        ReplayConfig config;
        std::string reportPath;
        std::string csvPath;
        std::string inputPath;
 
        // Parse arguments
        if (!parseArguments(argc, argv, config, reportPath, csvPath, inputPath)) {
            return 1;
        }

        // Set input file path
        config.dataFilePath = inputPath;
 
        // Create and run replay engine
        ReplayEngine engine(config);
        engine.start();
 
        // Output results to CSV
        ReportGenerator::generateCSV(engine, csvPath);
        std::cout << "✓ CSV written to: " << csvPath << "\n";
         
        // Print summary to console
        ReportGenerator::printSummary(engine);
 
        // Generate detailed report if requested
        if (!reportPath.empty()) {
            ReportGenerator::generateReport(engine, reportPath);
        }
 
        return 0;
 
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << "\n\n";
        return 1;
    }
}
