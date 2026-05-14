# Industrial Telemetry Replay & Failure Detection System

A high-performance C++17 command-line application that reads the **AI4I 2020 Predictive Maintenance dataset** from CSV, replays machine telemetry row-by-row as live data, calculates real-time machine health scores, detects warning/critical conditions, and generates comprehensive session reports.

## Project Overview

This system simulates a live industrial monitoring environment where:
- **Telemetry data** (temperatures, speeds, torque, wear) streams in real-time
- **Health scores** are calculated using a multi-factor algorithm
- **Failure detection** identifies warning and critical conditions
- **Session reports** provide detailed analytics on detection accuracy

## Features

### 1. CSV Parsing ✓
- Robust parsing of the AI4I 2020 Predictive Maintenance dataset
- Handles UTF-8 BOM and quoted CSV fields
- Validates 13-field structure (UID, Product ID, Type, Air Temp, Process Temp, Speed, Torque, Wear, failure flags)

### 2. Telemetry Replay Engine ✓
- Row-by-row playback with configurable delays
- Default 100ms delay between records (adjustable via `--delay`)
- Optional row limiting via `--limit` parameter
- Color-coded output for health status (Green=Healthy, Yellow=Warning, Red=Critical)
- Real-time progress display with formatted columns

### 3. Health Score Calculator ✓
Multi-factor weighted algorithm:
- **Tool Wear (35%)** - Most critical; accumulates from 0-250+min
- **Temperature (25%)** - Deviation from normal air/process temps
- **Torque Stress (25%)** - Deviation from normal 40Nm baseline
- **Rotational Speed (15%)** - Anomaly detection from 1500rpm baseline

Score Range: 0.0 (Critical) → 100.0 (Healthy)
Status Thresholds:
- `CRITICAL`: score < 40
- `WARNING`: 40 ≤ score < 70
- `HEALTHY`: score ≥ 70

### 4. Failure Detection ✓
Real-time detection of:
- **Critical Failures**: Score below critical threshold, extreme temps (>315K), excessive wear (>240min), extreme torque (>70Nm)
- **Warnings**: Score in warning range, rising temperatures, tool wear accumulation, elevated torque
- **Pattern Analysis**: Identifies root cause (wear, temperature, torque, or speed anomalies)
- **Accuracy Validation**: Compares detected conditions against actual recorded failures

### 5. Session Reporting ✓
Comprehensive output including:
- Executive summary (records processed, actual failures, detections)
- Detection events log (timestamp, score, status, description)
- Failure analysis (detection accuracy metrics)
- Performance metrics (throughput, processing time)
- Optional file output

## Dataset Information

**Source**: [AI4I 2020 Predictive Maintenance - Kaggle](https://www.kaggle.com/datasets/shivamb/machine-predictive-maintenance-classification)

**Structure**: 10,000 records with 13 columns:
- `UDI`: Unique ID
- `Product ID`: Machine identifier (Type M, L, or H)
- `Type`: Product type (M, L, H)
- `Air temperature [K]`: Ambient temperature
- `Process temperature [K]`: Operating temperature
- `Rotational speed [rpm]`: Spindle/motor speed
- `Torque [Nm]`: Mechanical load
- `Tool wear [min]`: Accumulated tool degradation
- `Machine failure`: Target label (0/1)
- `TWF`, `HDF`, `PWF`, `OSF`, `RNF`: Failure type indicators

## Building

### Requirements
- CMake 3.16+
- C++17 compiler (g++, clang, or MSVC)
- Standard C++ library (Threads)

### Build Instructions
```bash
git clone <repository>
cd industrial-telemetry-replay-cpp
mkdir build && cd build
cmake ..
make
```

### Output
Executable: `./industrial_monitor`

## Usage

### Basic Replay
```bash
./industrial_monitor data/ai4i2020.csv
```

### With Options
```bash
# Replay first 300 records with 50ms delay
./industrial_monitor data/ai4i2020.csv --limit 300 --delay 50

# Full replay with session report to file
./industrial_monitor data/ai4i2020.csv --report session_report.txt

# Quick replay with minimal output
./industrial_monitor data/ai4i2020.csv --delay 10 --quiet

# Suppress column headers
./industrial_monitor data/ai4i2020.csv --no-headers
```

### Command-Line Options
| Option | Argument | Default | Description |
|--------|----------|---------|-------------|
| `--delay` | milliseconds | 100 | Delay between records |
| `--limit` | number | all | Max records to process |
| `--report` | filepath | stdout | Write report to file |
| `--quiet` | - | false | Suppress verbose output |
| `--no-headers` | - | false | Hide column headers |
| `--help` | - | - | Show usage information |

## Example Output

```
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

  ╔═══════════════════════════════════════════════════════════╗
  ║  Industrial Telemetry Replay & Failure Detection System   ║
  ║  AI4I 2020 Predictive Maintenance Dataset                 ║
  ╚═══════════════════════════════════════════════════════════╝

  Session Configuration:
  ├─ Data Source: data/ai4i2020.csv
  ├─ Replay Delay: 100ms
  ├─ Row Limit: 300
  └─ Total Records Available: 10000

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

   Row     Health      Score  AirTemp(K) ProcTemp(K) Speed(rpm) Torque(Nm) Wear(min)            Event
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
     1    HEALTHY       91.7      298.1      308.6       1551       42.8         0                -
     2    HEALTHY       90.2      298.2      308.7       1408       46.3         3                -
     3    HEALTHY       89.7      298.1      308.5       1498       49.4         5                -
   ...
   287   WARNING        68.4      298.8      309.4       2015       58.2       142           WARNING
   ⚠ High torque stress (58.2Nm)
   288   WARNING        64.3      298.9      309.5       2041       62.1       145           WARNING
   ⚠ High torque stress (62.1Nm)
   289   CRITICAL       38.2      299.1      310.1       2089       68.5       248          CRITICAL
   ⚠ Critical tool wear (248min)

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

  Session Summary:
  ├─ Records Replayed: 300
  ├─ Actual Failures Detected: 12
  ├─ Critical Warnings: 23
  ├─ Warnings: 87
  ├─ Total Replay Time: 30245ms
  ├─ Detection Accuracy: 94.2%
  └─ Avg Record Processing: 100.8ms

──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
```

## Architecture

### Core Components

```
main.cpp
  ├── ReplayEngine (replay_engine.h/cpp)
  │   ├── CsvParser (csv_parser.h/cpp)
  │   ├── HealthCalculator (health_calculator.h/cpp)
  │   └── FailureDetector (failure_detector.h/cpp)
  └── ReportGenerator (report_generator.h/cpp)
```

### Data Flow
1. **CSV Loading**: CsvParser reads file → TelemetryRecord objects
2. **Replay Loop**: Each record is processed with configurable delay
3. **Health Calculation**: Multi-factor algorithm scores machine health
4. **Failure Detection**: Pattern matching identifies warning/critical states
5. **Report Generation**: Statistics and events are formatted and output

## Algorithm Details

### Health Score Calculation

For each record:
1. Calculate component scores (0-100):
   - `temperatureScore`: Penalizes deviation from normal temps
   - `speedScore`: Detects rotational anomalies
   - `torqueScore`: Evaluates mechanical stress
   - `wearScore`: Tracks cumulative tool degradation

2. Weighted average:
   ```
   health = (wear×0.35) + (temp×0.25) + (torque×0.25) + (speed×0.15)
   ```

3. Determine status:
   - score < 40 → CRITICAL
   - 40 ≤ score < 70 → WARNING
   - score ≥ 70 → HEALTHY

### Failure Detection Strategy

**Critical Conditions** (immediate alert):
- Health score < 40
- Process temperature > 315K
- Tool wear > 240 minutes
- Torque > 70 Nm

**Warning Conditions** (trend alert):
- Health score 40-70
- Process temperature > 312K
- Tool wear > 180 minutes
- Torque > 55 Nm

## File Structure

```
industrial-telemetry-replay-cpp/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── csv_parser.h
│   ├── telemetry_record.h
│   ├── health_calculator.h
│   ├── failure_detector.h
│   ├── replay_engine.h
│   └── report_generator.h
└── src/
    ├── main.cpp
    ├── csv_parser.cpp
    ├── telemetry_record.cpp
    ├── health_calculator.cpp
    ├── failure_detector.cpp
    ├── replay_engine.cpp
    └── report_generator.cpp
```

## Performance

- **Throughput**: ~100-200 records/sec (depending on delay)
- **Memory Usage**: ~10-15MB for full dataset in memory
- **CPU**: Minimal; processing time negligible vs. I/O

## Testing & Validation

The detector includes built-in accuracy validation:
- Compares detected conditions against actual recorded failure flags
- Tracks warning precision and recall
- Reports accuracy percentage in session summary

## Future Enhancements

Potential improvements:
- Machine learning-based anomaly detection
- Streaming/online processing (no full dataset preload)
- Statistical threshold auto-tuning
- Predictive maintenance (forecast failures before occurrence)
- Multi-machine correlation analysis
- Web dashboard for real-time monitoring
- Database backend for historical analysis

## License

[Specify your license here]

## Author

[Your name/organization]

## References

- AI4I 2020 Dataset: https://www.kaggle.com/datasets/shivamb/machine-predictive-maintenance-classification
- Predictive Maintenance: https://en.wikipedia.org/wiki/Predictive_maintenance
