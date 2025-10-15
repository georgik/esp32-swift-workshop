# ESP32-C6 Swift Workshop

A comprehensive workshop demonstrating **Swift 6.2 Embedded programming** on ESP32-C6 microcontrollers with IoT integration.

## 🚀 Quick Start

### Prerequisites
- **ESP-IDF 6.0+** with Swift support
- **Swift 6.2+** with Embedded Swift support
- **SwiftLint** and **swift-format** for code quality
- **CMake 3.29+** for Swift integration

### Setup
```bash
# Clone and install tools
git clone <repository-url>
cd esp32-swift-workshop
make install-tools

# Verify installation
swift --version
swiftlint version
swift-format --version
```

## 📚 Workshop Structure

1. **[Assignment 1](content/assignments/assignment-1.md)**: Environment Setup
2. **[Assignment 2](content/assignments/assignment-2.md)**: MQTT Gateway in C  
3. **[Assignment 3](content/assignments/assignment-3.md)**: Swift IoT Client
4. **[Assignment 4](content/assignments/assignment-4.md)**: Device Communication
5. **[Assignment 5](content/assignments/assignment-5.md)**: Testing & Monitoring

## 🏗️ Project Architecture

### ESP32-C6 MQTT Gateway (C)
- **Purpose**: WiFi infrastructure + MQTT broker access
- **Features**: Auto-reconnection, mDNS discovery, status monitoring
- **Build**: `cd esp32-c6-mqtt-gateway && idf.py build`

### ESP32-C6 Swift Client (Swift + C Hybrid)
- **Purpose**: Swift Embedded demonstration with NeoPixel control
- **Features**: RISC-V compilation, C-Swift bridging, IoTCraft integration
- **Build**: `cd esp32-c6-swift-client && idf.py build`

## 🎨 Pure Swift Development Approach

This workshop uses **pure Swift tooling** - no legacy Makefile dependencies:

### Swift Linting Commands
```bash
# Format all Swift files  
./Scripts/lint.swift format

# Run comprehensive linting (SwiftLint + swift-format)
./Scripts/lint.swift lint

# Check formatting (CI-friendly)
./Scripts/lint.swift check

# Show help
./Scripts/lint.swift help
```

### ESP-IDF Project Commands
```bash
# Build projects
idf.py build                          # Build current project
cd esp32-c6-mqtt-gateway && idf.py build      # Build MQTT Gateway
cd esp32-c6-swift-client && idf.py build      # Build Swift Client

# Hardware interaction
idf.py flash                          # Flash current project  
idf.py monitor                        # Monitor serial output
idf.py flash monitor                  # Flash and monitor
```

### Configuration Files
- **`.swiftlint.yml`**: Embedded-specific SwiftLint rules (relaxed line length, GPIO naming)
- **`.swift-format`**: Apple's formatting standards with 2-space indentation
- **`Scripts/lint.swift`**: Unified Swift linting interface

### Why Pure Swift Approach?
- ✅ **Modern tooling**: Uses SwiftLint + swift-format + idf.py directly
- ✅ **No legacy dependencies**: No Makefile complexity
- ✅ **Better embedded rules**: Custom rules for GPIO naming, memory allocation
- ✅ **Native Swift scripting**: Development tools written in Swift
- ✅ **Simplified workflow**: Direct ESP-IDF commands + Swift linting
- ✅ **Clear separation**: Swift tools for Swift code, ESP-IDF tools for embedded builds

## 🔧 Development Workflow

### Daily Development
```bash
# 1. Make Swift changes...

# 2. Format and lint Swift code
./Scripts/lint.swift format
./Scripts/lint.swift lint

# 3. Build ESP-IDF projects
cd esp32-c6-mqtt-gateway && idf.py build    # Build gateway
cd esp32-c6-swift-client && idf.py build    # Build Swift client

# 4. Flash and test on hardware
cd esp32-c6-swift-client && idf.py flash monitor

# 5. Run integration tests (if available)
cd tests && python3 test_framework.py
```

### Pre-commit Hooks
Automatically runs on `git commit`:
- **SwiftLint**: Comprehensive Swift style checking
- **swift-format**: Formatting validation  
- **ESP-IDF**: Configuration validation
- **Markdown**: Content and link validation

## 🧪 Testing & Monitoring

### Integration Testing
```bash
# Python integration tests
cd tests && python3 test_framework.py

# Swift code quality
./Scripts/lint.swift lint

# Manual build verification
cd esp32-c6-mqtt-gateway && idf.py build
cd esp32-c6-swift-client && idf.py build
```

### Real-time Monitoring
- **Python Dashboard**: Flask + SocketIO monitoring interface
- **MQTT Testing**: Automated device connectivity testing
- **Performance Metrics**: Memory, latency, and throughput monitoring

## 📦 CI/CD Pipeline

### GitHub Actions
- **Swift Quality**: SwiftLint + swift-format validation
- **ESP-IDF Builds**: Both projects with artifact generation
- **Content Validation**: Workshop assignments and examples
- **Release Automation**: Workshop package creation

### Local CI Simulation
```bash
# Run complete pipeline manually
./Scripts/lint.swift lint                    # Swift linting
cd esp32-c6-mqtt-gateway && idf.py build    # Build gateway
cd esp32-c6-swift-client && idf.py build    # Build Swift client  
cd tests && python3 test_framework.py       # Integration tests
```

## 🎯 Key Features

### Swift Embedded Programming
- **RISC-V Target**: `riscv32-none-none-eabi` compilation
- **Memory Optimized**: Custom partition table, heap allocation strategies
- **Hardware Integration**: GPIO control, NeoPixel LED management
- **C Interoperability**: Seamless ESP-IDF API bridging

### IoTCraft Ecosystem
- **MQTT Integration**: Device registration, status reporting
- **Desktop Client**: 3D visualization and device control
- **Real-time Communication**: Sub-50ms command response

### Production-Ready Tooling
- **Code Quality**: SwiftLint + swift-format integration
- **Automation**: Pre-commit hooks, CI/CD pipelines
- **Documentation**: Comprehensive assignment materials
- **Testing**: Integration testing framework

## 🤝 Contributing

See [DEVELOPMENT.md](DEVELOPMENT.md) for detailed contributor guidelines.

### Quick Contribution Workflow
```bash
# 1. Format Swift code
./Scripts/lint.swift format

# 2. Run quality checks  
./Scripts/lint.swift lint

# 3. Test builds
cd esp32-c6-mqtt-gateway && idf.py build
cd esp32-c6-swift-client && idf.py build

# 4. Run pre-commit checks
pre-commit run --all-files

# 5. Create pull request
```

## 📖 Documentation

- **[WARP.md](WARP.md)**: AI assistant context and project overview
- **[DEVELOPMENT.md](DEVELOPMENT.md)**: Contributor and maintainer guide  
- **[Assignments](content/assignments/)**: Step-by-step workshop materials
- **[Testing Guide](tests/)**: Integration testing documentation

---

**Note**: This workshop demonstrates modern Swift Embedded development with production-quality tooling. The combination of Swift's native linting tools with ESP-IDF's build system provides the best of both ecosystems.