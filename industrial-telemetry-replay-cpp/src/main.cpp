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
    std::cout << "  " << programName << " <csv_file> [OPTIONS]\n";
    std::cout << "\n";
    std::cout << "ARGUMENTS:\n";
    std::cout << "  <csv_file>             Path to the AI4I 2020 CSV dataset\n";
    std::cout << "\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  --delay <ms>           Delay between records in milliseconds (default: 100)\n";
    std::cout << "  --limit <n>            Maximum number of rows to process (default: all)\n";
    std::cout << "  --report <file>        Write session report to file (default: stdout)\n";
    std::cout << "  --quiet                Suppress verbose output during replay\n";
    std::cout << "  --no-headers           Hide column headers in output\n";
    std::cout << "  --help                 Show this help message\n";
    std::cout << "\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  # Replay first 300 records with 50ms delay\n";
    std::cout << "  " << programName << " data/ai4i2020.csv --limit 300 --delay 50\n";
    std::cout << "\n";
    std::cout << "  # Full replay with session report to file\n";
    std::cout << "  " << programName << " data/ai4i2020.csv --report session_report.txt\n";
    std::cout << "\n";
    std::cout << "  # Quick replay with minimal output\n";
    std::cout << "  " << programName << " data/ai4i2020.csv --delay 10 --quiet\n";
    std::cout << "\n";
}

/**
 * Parse command-line arguments
 */
bool parseArguments(int argc, char* argv[], ReplayConfig& config, std::string& reportPath) {
    if (argc < 2) {
        printUsage(argv[0]);
        return false;
    }

    config.dataFilePath = argv[1];

    // Check for help flag first
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return false;
        }
    }

    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--delay" && i + 1 < argc) {
            config.delayMs = std::stoi(argv[++i]);
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

        // Parse arguments
        if (!parseArguments(argc, argv, config, reportPath)) {
            return 1;
        }

        // Create and run replay engine
        ReplayEngine engine(config);
        engine.start();

        // Generate and output report
        ReportGenerator::generateReport(engine, reportPath);

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << "\n\n";
        return 1;
    }
}
