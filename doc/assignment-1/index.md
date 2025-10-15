---
title: "ESP32-C6 Swift Embedded Workshop - Assignment 1: Environment Setup"
date: 2024-10-15T00:00:00+01:00
showTableOfContents: false
series: ["WS002"]
series_order: 2
showAuthor: false
---

## Assignment 1: Environment Setup

---

This assignment will guide you through setting up the complete development environment for Swift Embedded development on ESP32-C6. You'll install ESP-IDF, Swift toolchain, and configure your workspace for the workshop.

### Prerequisites Verification

Before starting, ensure you have:

- **macOS 12+**, **Linux (Ubuntu 20.04+)**, or **Windows 10+ with WSL2**
- **ESP32-C6 Development Board** (ESP32-C6-DevKitC-1 or ESP32-C6-DevKitM-1)
- **USB-C Cable** for programming and debugging
- **Admin/sudo privileges** for installing development tools
- **Stable internet connection** for downloading toolchains

### Step 1: Install ESP-IDF 6.0+

ESP-IDF (Espressif IoT Development Framework) is the foundation for ESP32-C6 development.

#### macOS Installation

```bash
# Install prerequisites
brew install cmake ninja ccache dfu-util

# Create ESP development directory
mkdir -p ~/esp
cd ~/esp

# Clone ESP-IDF
git clone -b release/v5.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf

# Install ESP-IDF
./install.sh esp32c6

# Set up environment (add to ~/.zshrc or ~/.bash_profile)
source ~/esp/esp-idf/export.sh
```

#### Linux (Ubuntu) Installation

```bash
# Update package manager and install prerequisites
sudo apt-get update
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

# Create ESP development directory
mkdir -p ~/esp
cd ~/esp

# Clone ESP-IDF
git clone -b release/v5.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf

# Install ESP-IDF
./install.sh esp32c6

# Set up environment (add to ~/.bashrc)
source ~/esp/esp-idf/export.sh
```

#### Verify ESP-IDF Installation

```bash
# Check ESP-IDF version
idf.py --version

# Expected output: ESP-IDF v5.2.x or later
```

### Step 2: Install Swift 6.2+ with Embedded Support

Swift Embedded requires specific Swift toolchain builds with cross-compilation support.

#### macOS Installation

```bash
# Install Swift 6.2+ from swift.org
# Download from: https://swift.org/download/#releases
# Choose: Swift 6.2 Release for macOS

# Alternatively, use Swift toolchain with experimental features
# Download Swift Development Snapshot that supports embedded targets

# Verify Swift installation
swift --version

# Expected: Swift version 6.2.x or later
```

#### Swift Cross-compilation Setup

For RISC-V targets, you may need additional configuration:

```bash
# Check available targets
swift --print-target-info

# The output should include riscv32-unknown-none-eabi support
```

### Step 3: Install CMake 3.29+

CMake 3.29+ is required for Swift+ESP-IDF integration.

#### macOS

```bash
# Install via Homebrew
brew install cmake

# Verify version
cmake --version

# Should show version 3.29.0 or later
```

#### Linux

```bash
# Remove old CMake if installed
sudo apt-get remove cmake

# Install latest CMake
wget https://github.com/Kitware/CMake/releases/download/v3.29.0/cmake-3.29.0-linux-x86_64.sh
chmod +x cmake-3.29.0-linux-x86_64.sh
sudo ./cmake-3.29.0-linux-x86_64.sh --skip-license --prefix=/usr/local

# Verify version
cmake --version
```

### Step 4: Set Up Development Workspace

Create and configure your workshop workspace.

```bash
# Create workshop directory
mkdir -p ~/esp32-swift-workshop
cd ~/esp32-swift-workshop

# Clone or create workshop projects
git clone <workshop-repository-url> .
# OR create the directory structure manually:
mkdir -p esp32-c6-mqtt-gateway esp32-c6-swift-client

# Set up environment variables (add to your shell profile)
export ESP_WORKSHOP_PATH=~/esp32-swift-workshop
export IDF_PATH=~/esp/esp-idf
```

### Step 5: Configure USB Drivers and Permissions

#### macOS
No additional drivers needed - USB CDC is supported natively.

#### Linux
Set up udev rules for ESP32-C6 USB access:

```bash
# Create udev rules file
sudo tee /etc/udev/rules.d/99-esp32.rules > /dev/null <<EOF
SUBSYSTEM=="usb", ATTR{idVendor}=="303a", ATTR{idProduct}=="1001", MODE="0666"
SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", MODE="0666"
EOF

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add user to dialout group
sudo usermod -a -G dialout $USER
```

### Step 6: Verify Hardware Connection

Connect your ESP32-C6 development board and verify communication.

```bash
# Connect ESP32-C6 via USB-C cable
# Check if device is detected

# macOS
ls /dev/cu.usbmodem*

# Linux  
ls /dev/ttyACM*

# You should see a device like /dev/cu.usbmodem1101 or /dev/ttyACM0
```

### Step 7: Test Basic ESP-IDF Functionality

Verify your ESP-IDF installation works with ESP32-C6.

```bash
# Navigate to ESP-IDF examples
cd $IDF_PATH/examples/get-started/hello_world

# Configure for ESP32-C6
idf.py set-target esp32c6

# Build the example
idf.py build

# Flash and monitor (replace with your port)
idf.py -p /dev/cu.usbmodem* flash monitor

# Expected output:
# Hello world!
# This is esp32c6 chip with 1 CPU core(s), WiFi/BLE, silicon revision 0, 2MB external flash
```

Press `Ctrl+]` to exit the monitor.

### Step 8: Install Additional Tools

Install helpful development tools for the workshop.

#### Wokwi Simulator Setup

Wokwi is a powerful online ESP32 simulator that allows you to test your code without physical hardware. It's particularly useful for:

- **Prototyping**: Test circuit connections and component interactions
- **Debugging**: Debug code with full access to variables and memory
- **Learning**: Visualize how your code interacts with hardware components
- **Sharing**: Share projects with others via simple URLs
- **CI/CD**: Automated testing in continuous integration pipelines

**Setting up Wokwi:**

1. **Install Wokwi CLI** (for local development):
   ```bash
   # Install Wokwi CLI
   npm install -g @wokwi/cli
   
   # Verify installation
   wokwi --version
   ```

2. **Alternative: Use Wokwi VS Code Extension**:
   - Install "Wokwi Simulator" extension in VS Code
   - Provides integrated simulation environment
   - Supports debugging and live code editing

3. **Online Wokwi** (no installation needed):
   - Visit [wokwi.com](https://wokwi.com)
   - Upload your compiled `.bin` files
   - Configure hardware components visually

#### MQTT Client Tools

```bash
# macOS
brew install mosquitto

# Linux
sudo apt-get install mosquitto-clients

# Test MQTT tools
mosquitto_pub --help
mosquitto_sub --help
```

#### Serial Monitor Alternatives

```bash
# Install minicom (alternative to idf.py monitor)
# macOS
brew install minicom

# Linux
sudo apt-get install minicom
```

### Step 9: Configure Shell Environment

Add permanent environment configuration to your shell profile.

#### For macOS (zsh)

```bash
# Edit ~/.zshrc
echo 'export IDF_PATH=~/esp/esp-idf' >> ~/.zshrc
echo 'source $IDF_PATH/export.sh' >> ~/.zshrc
echo 'export ESP_WORKSHOP_PATH=~/esp32-swift-workshop' >> ~/.zshrc

# Reload shell
source ~/.zshrc
```

#### For Linux (bash)

```bash
# Edit ~/.bashrc
echo 'export IDF_PATH=~/esp/esp-idf' >> ~/.bashrc
echo 'source $IDF_PATH/export.sh' >> ~/.bashrc
echo 'export ESP_WORKSHOP_PATH=~/esp32-swift-workshop' >> ~/.bashrc

# Reload shell
source ~/.bashrc
```

### Step 10: Verification Checklist

Run through this checklist to ensure everything is properly installed:

```bash
# 1. ESP-IDF version check
idf.py --version
# ✅ Should show ESP-IDF v5.2.x

# 2. Swift version check  
swift --version
# ✅ Should show Swift 6.2.x or later

# 3. CMake version check
cmake --version
# ✅ Should show CMake 3.29.x or later

# 4. ESP32-C6 detection
idf.py flash --dry-run -p /dev/cu.usbmodem*
# ✅ Should detect ESP32-C6 chip

# 5. MQTT tools check
mosquitto_pub --help > /dev/null && echo "MQTT tools OK"
# ✅ Should print "MQTT tools OK"
```

### Troubleshooting Common Issues

#### ESP-IDF Installation Issues

**Problem**: `idf.py: command not found`
```bash
# Solution: Ensure ESP-IDF export script is sourced
source ~/esp/esp-idf/export.sh
```

**Problem**: Permission denied when accessing USB device
```bash
# macOS: Try with sudo temporarily
sudo idf.py -p /dev/cu.usbmodem* flash

# Linux: Check user groups and udev rules
groups $USER  # Should include dialout
sudo udevadm control --reload-rules
```

#### Swift Installation Issues

**Problem**: Swift doesn't support riscv32 target
```bash
# Check available targets
swift --print-target-info

# If RISC-V support is missing, try Swift development snapshots
# from swift.org that include experimental features
```

#### CMake Version Issues

**Problem**: CMake version too old
```bash
# Check current version
cmake --version

# Uninstall old version and install 3.29+
# Follow Step 3 instructions for your platform
```

### Development Environment Verification

Create a simple test to verify your complete setup:

```bash
# Create test directory
mkdir ~/esp-swift-test
cd ~/esp-swift-test

# Create minimal CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.29)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp-swift-test)
EOF

# Create main component directory
mkdir -p main
cat > main/CMakeLists.txt << 'EOF'
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
)
EOF

# Create simple main.c
cat > main/main.c << 'EOF'
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    printf("ESP32-C6 Swift Workshop Environment Test\n");
    printf("ESP-IDF Ready!\n");
    printf("Swift Embedded development environment configured.\n");
}
EOF

# Build and test
idf.py set-target esp32c6
idf.py build

# If build succeeds, your environment is ready!
echo "✅ Development environment setup complete!"
```

## Next Step

Congratulations! Your development environment is now configured for Swift Embedded development on ESP32-C6. You're ready to build the MQTT gateway infrastructure.

[Assignment 2: MQTT Gateway (C Implementation)](../assignment-2)