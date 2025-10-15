---
title: "ESP32-C6 Swift Embedded Workshop - Assignment 3: Swift Client Development"
date: 2024-10-15T00:00:00+01:00
showTableOfContents: false
series: ["WS002"]
series_order: 4
showAuthor: false
---

## Assignment 3: Swift Client Development

---

In this assignment, you'll build a Swift Embedded application for ESP32-C6 that demonstrates the power of modern Swift development on microcontrollers. This client will feature NeoPixel LED control, MQTT communication, and real-time device interaction.

### Learning Objectives

By the end of this assignment, you will:

- Understand Swift Embedded development patterns and constraints
- Create C-Swift interoperability using bridging headers
- Implement hardware control in Swift (NeoPixel LEDs)
- Build MQTT client functionality with Swift application logic
- Debug Swift applications using both hardware and Wokwi simulation
- Optimize Swift code for embedded resource constraints

### Assignment Overview

The Swift Client implements a smart IoT device with:

- **Swift Application Logic**: Main device behavior written in Swift
- **C-Swift Bridge**: Interoperability layer for ESP-IDF APIs
- **NeoPixel LED Control**: Hardware abstraction for visual feedback
- **MQTT Integration**: Device communication and remote control
- **Device Registration**: Automatic announcement to IoT ecosystem

### Step 1: Examine Swift Embedded Project Structure

Navigate to the Swift client project:

```bash
cd $ESP_WORKSHOP_PATH/esp32-c6-swift-client

# Examine the project structure
tree .
```

**Project Structure:**
```
esp32-c6-swift-client/
├── CMakeLists.txt              # Project configuration (Swift support)
├── partitions.csv              # Custom 1.5MB app partition for Swift
├── sdkconfig.defaults          # ESP-IDF configuration for Swift
└── main/
    ├── CMakeLists.txt          # Complex Swift+C compilation setup
    ├── idf_component.yml       # LED strip component dependency
    ├── Main.swift              # Swift application entry point
    ├── LedStrip.swift          # Swift NeoPixel abstraction
    ├── esp_swift_wrapper.c     # C-Swift interoperability bridge
    ├── esp_swift_wrapper.h     # C function declarations  
    └── BridgingHeader.h        # Swift-C bridging header
```

### Step 2: Understand Swift Embedded Architecture

#### Swift Embedded Constraints

This project demonstrates Swift Embedded development, which has specific constraints:

- **No Foundation**: No `String`, `Array`, or standard library types
- **Manual Memory Management**: No automatic reference counting (ARC)
- **Static Dispatch**: All method calls resolved at compile time
- **No Runtime Reflection**: Minimal runtime footprint
- **Hardware Integration**: Direct interaction with embedded hardware

#### Multi-Language Architecture

```
┌─────────────────────────────────────┐
│           Swift Layer               │  ← Application Logic
├─────────────────────────────────────┤
│  Main.swift: Device behavior        │
│  LedStrip.swift: Hardware wrapper   │  
│  @_cdecl callbacks for C interop    │
└─────────────────┬───────────────────┘
                  │ Bridging Header
┌─────────────────▼───────────────────┐
│            C Bridge Layer           │  ← ESP-IDF Integration
├─────────────────────────────────────┤
│  esp_swift_wrapper.c                │
│  - WiFi management                  │
│  - MQTT client handling             │
│  - Hardware initialization          │
└─────────────────┬───────────────────┘
                  │ ESP-IDF APIs
┌─────────────────▼───────────────────┐
│           ESP-IDF Framework         │  ← Hardware Abstraction
└─────────────────────────────────────┘
```

### Step 3: Examine Swift Application Code

#### Main.swift - Application Entry Point

```swift path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-swift-client/main/Main.swift start=21
@_cdecl("app_main")
func main() {
    print("=== ESP32-C6 Swift IoT Client v3.0 ===")
    
    // Initialize NeoPixel LED strip
    globalLedStrip = LedStrip(gpioPin: 8, maxLeds: 1)
    globalLedStrip?.showStatus(isOnline: false)
    
    // Initialize WiFi and MQTT
    wifi_init()
    wifi_connect("your-wifi-ssid".withCString { $0 }, 
                 "your-password".withCString { $0 })
    
    mqtt_client_init()
    mqtt_client_start()
    
    // Main application loop
    while true {
        // Device announcement and status updates
        mqtt_publish_device_announcement()
        vTaskDelay(100 / (1000 / UInt32(configTICK_RATE_HZ)))
    }
}
```

Key Swift Embedded patterns:
- **`@_cdecl`**: Exports Swift functions to C-compatible symbols
- **Manual C String Conversion**: Using `withCString` for C interop
- **No String Literals**: Direct C function calls instead of Swift strings
- **FreeRTOS Integration**: Using `vTaskDelay` for task scheduling

#### LedStrip.swift - Hardware Abstraction

```swift path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-swift-client/main/LedStrip.swift start=1
struct LedStrip {
    private let stripHandle: UnsafeRawPointer?
    
    init(gpioPin: UInt32, maxLeds: UInt32) {
        stripHandle = led_strip_init(gpioPin, maxLeds)
    }
    
    func showStatus(isOnline: Bool) {
        if isOnline {
            setAll(color: .green)
        } else {
            setAll(color: .red) 
        }
        refresh()
    }
    
    func setAll(color: Color) {
        led_strip_set_pixel_color(stripHandle, 0, 
                                  color.red, color.green, color.blue)
    }
}
```

### Step 4: Configure WiFi Credentials for Swift Client

Unlike the C gateway, the Swift client has WiFi credentials hardcoded in Swift:

```bash
# Edit WiFi credentials in Main.swift
nano main/Main.swift

# Find and update these lines:
let ssid = "your-wifi-network"
let password = "your-wifi-password"
```

**Important for Wokwi Simulation**: 
For Wokwi testing, use the default credentials:
```swift
let ssid = "Wokwi-GUEST"
let password = ""
```

### Step 5: Build the Swift Client

The Swift client uses a complex CMake configuration to cross-compile Swift for RISC-V:

```bash
# Set ESP32-C6 target
idf.py set-target esp32c6

# Build the project (this may take several minutes for Swift compilation)
idf.py build
```

**Expected Build Output:**
```
Swift compilation target: riscv32-none-none-eabi
Compiling Swift sources for Embedded Swift...
Linking Swift objects with ESP-IDF...
Project build complete. To flash, run:
  idf.py -p /dev/cu.usbmodem* flash monitor

Binary size: ~1.1MB (Swift runtime + application)
```

**Swift Compilation Details:**
- **Target Architecture**: `riscv32-none-none-eabi`
- **Optimization**: `-Osize` (optimize for binary size)
- **Mode**: `-enable-experimental-feature Embedded`
- **Whole Module**: `-wmo` (whole module optimization)
- **Memory Layout**: Custom partition table with 1.5MB app partition

### Step 6: Test with Wokwi Simulation (Recommended First)

Before flashing to hardware, test with Wokwi simulation:

#### Wokwi Configuration for Swift Client

The project includes `wokwi.toml` with NeoPixel simulation:

```toml
[wokwi]
version = 1
firmware = 'build/esp32-c6-swift-client.bin'
elf = 'build/esp32-c6-swift-client.elf'

[[wokwi.parts]]
type = 'wokwi-esp32-c6'
id = 'esp'

[[wokwi.parts]]
type = 'wokwi-neopixel'  
id = 'neo1'
attrs = { pin = '8', pixels = '1' }

[[wokwi.connections]]
from = 'esp:8'
to = 'neo1:DIN'

[[wokwi.connections]]  
from = 'esp:3V3'
to = 'neo1:VCC'

[[wokwi.connections]]
from = 'esp:GND.1' 
to = 'neo1:GND'
```

#### Run Wokwi Simulation

```bash
# Start Wokwi simulation
wokwi --timeout 60000

# Expected simulation behavior:
# 1. NeoPixel starts RED (offline)
# 2. Changes to YELLOW (connecting)
# 3. Changes to GREEN (online)
# 4. Responds to MQTT commands with WHITE/BLACK
```

#### Wokwi Swift Simulation Benefits

- **Visual LED Feedback**: See NeoPixel color changes in real-time
- **Swift Debugging**: Set breakpoints in Swift code
- **Memory Analysis**: Monitor Swift memory usage
- **Network Testing**: Test MQTT without external broker
- **Fast Iteration**: No hardware flashing required

### Step 7: Flash and Monitor on Hardware

After successful Wokwi testing, deploy to ESP32-C6 hardware:

```bash
# Flash to hardware (replace with your port)
idf.py -p /dev/cu.usbmodem* flash monitor
```

**Expected Serial Output:**
```
=== ESP32-C6 Swift IoT Client v3.0 ===
Production-ready with ESP-IDF integration
✅ NVS initialized
✅ LED strip initialized on GPIO8 with 1 LEDs
✅ WiFi connected successfully
SSID: your-wifi-network
✅ MQTT connected successfully
📡 Device ID: esp32c6-aabbccddeeff
🚀 Ready to receive MQTT commands!
📋 Subscribed to topics:
   - home/esp32c6-aabbccddeeff/light
   - home/esp32c6-aabbccddeeff/status
```

### Step 8: Test Swift Device Functionality

#### Test 1: Visual LED Status

Observe the NeoPixel LED behavior:

1. **Red**: Device startup/offline
2. **Yellow**: Connecting to WiFi/MQTT  
3. **Green**: Successfully online
4. **White**: Light ON command received
5. **Black**: Light OFF command received

#### Test 2: MQTT Device Control

Test the device using MQTT commands:

```bash
# Subscribe to device announcements
mosquitto_sub -h localhost -t "devices/announce"

# Expected device announcement:
# {"device_id":"esp32c6-aabbccddeeff","device_type":"lamp","state":"online","location":{"x":1.0,"y":0.5,"z":2.0}}

# Control the device light
mosquitto_pub -h localhost -t "home/esp32c6-aabbccddeeff/light" -m "ON"
# LED should turn WHITE

mosquitto_pub -h localhost -t "home/esp32c6-aabbccddeeff/light" -m "OFF"  
# LED should turn BLACK (off)
```

#### Test 3: Device Status Updates

```bash
# Subscribe to status updates
mosquitto_sub -h localhost -t "home/esp32c6-aabbccddeeff/status"

# Expected periodic status:
# {"device_id":"esp32c6-aabbccddeeff","state":"online","light_status":"ON","timestamp":1697123456}
```

### Step 9: Understanding Swift-C Interoperability

#### Bridging Header (BridgingHeader.h)

```c path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-swift-client/main/BridgingHeader.h start=1
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_swift_wrapper.h"

// Expose ESP-IDF and custom C functions to Swift
```

#### C-Swift Wrapper Functions

```c path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-swift-client/main/esp_swift_wrapper.c start=1
// WiFi initialization wrapper
int wifi_init(void) {
    // ESP-IDF WiFi initialization
    return esp_wifi_init() == ESP_OK ? 0 : -1;
}

// MQTT client wrapper with Swift-friendly interface
int mqtt_client_init(void) {
    // ESP-IDF MQTT client setup
    return esp_mqtt_client_init() == ESP_OK ? 0 : -1;
}

// LED strip wrapper for Swift
void* led_strip_init(uint32_t gpio_pin, uint32_t max_leds) {
    // ESP-IDF LED strip initialization
    return led_strip_new_rmt_device();
}
```

#### Swift Callback Registration

```swift
// Register LED control callback with C layer  
let ledCallback: @convention(c) (Bool) -> Void = { isOn in
    ledControlCallback(isOn: isOn)
}
register_led_control_callback(ledCallback)
```

### Step 10: Swift Embedded Best Practices

#### Memory Management

```swift
// ✅ Good: Stack allocation for small data
var buffer = (UInt8(0), UInt8(0), UInt8(0), UInt8(0))

// ❌ Bad: No heap allocation in Embedded Swift
// var array = [UInt8](repeating: 0, count: 1024)

// ✅ Good: Use C malloc for larger allocations
let pointer = UnsafeMutablePointer<UInt8>.allocate(capacity: 1024)
defer { pointer.deallocate() }
```

#### C String Handling

```swift
// ✅ Good: Temporary C string conversion
ssid.withCString { ssidPtr in
    password.withCString { passwordPtr in
        wifi_connect(ssidPtr, passwordPtr)
    }
}

// ❌ Bad: No Foundation String methods available
// let uppercased = ssid.uppercased()
```

#### Hardware Integration Patterns

```swift
struct HardwareDevice {
    private let handle: UnsafeRawPointer?
    
    init(config: DeviceConfig) {
        handle = device_init_c_function(config)
    }
    
    func performOperation() -> Bool {
        guard let handle = handle else { return false }
        return device_operation_c_function(handle) == 0
    }
    
    deinit {
        if let handle = handle {
            device_cleanup_c_function(handle)
        }
    }
}
```

### Step 11: Debugging Swift Embedded Applications

#### Serial Debug Output

```swift
// Use print() for debug output (maps to printf)
print("Debug: Device state changed to: \(isOnline ? "online" : "offline")")

// For formatted output, use C-style formatting through wrapper
debug_log("WiFi RSSI: %d dBm".withCString { $0 }, wifi_get_rssi())
```

#### Wokwi Debugging Features

- **Breakpoints**: Set breakpoints directly in Swift code
- **Variable Inspection**: View Swift variables and memory
- **Step Debugging**: Step through Swift and C code
- **Memory Viewer**: Analyze memory usage patterns
- **Logic Analyzer**: Monitor GPIO signals and timing

#### Hardware Debugging

```bash
# Monitor with detailed logging
idf.py -p /dev/cu.usbmodem* monitor --print_filter="*:DEBUG"

# Check memory usage
idf.py size

# Analyze binary composition
idf.py size-components
```

### Step 12: Performance Optimization

#### Binary Size Optimization

The Swift client uses several optimization techniques:

1. **Whole Module Optimization**: `-wmo` flag
2. **Size Optimization**: `-Osize` flag  
3. **Custom Partition Table**: Dedicated 1.5MB app partition
4. **Component Stripping**: Remove unused Swift runtime features

#### Runtime Performance

- **Stack Usage**: Monitor task stack with `uxTaskGetStackHighWaterMark`
- **Heap Usage**: Use `esp_get_free_heap_size()` for monitoring
- **CPU Usage**: Profile with ESP-IDF performance analysis tools

### Troubleshooting Common Issues

#### Swift Compilation Errors

**Problem**: "Swift compiler not found"
```bash
# Verify Swift installation
swift --version

# Check PATH includes Swift toolchain
echo $PATH | grep swift
```

**Problem**: "RISC-V target not supported"
```bash
# Check available targets
swift --print-target-info

# Install Swift development snapshot with embedded support
# Visit swift.org for latest toolchain downloads
```

#### Memory Issues

**Problem**: "App partition too small"
```bash
# Check partition configuration
cat partitions.csv

# The project uses custom 1.5MB app partition
# If still too small, increase partition size
```

**Problem**: Stack overflow in Swift code
```bash
# Monitor stack usage
print("Stack watermark: \(uxTaskGetStackHighWaterMark(nil))")

# Increase stack size in C wrapper if needed
xTaskCreate(swift_main_task, "swift_main", 8192, NULL, 5, NULL);
```

#### Hardware Integration Issues

**Problem**: NeoPixel not responding
```bash
# Verify GPIO 8 configuration
# Check power supply (3.3V to NeoPixel VCC)
# Test with simple C LED blink first
```

**Problem**: MQTT commands not received
```bash
# Check topic subscription in serial output
# Verify device ID generation
# Test with mosquitto_pub/sub tools
```

### Understanding Swift's Role in IoT

This Swift client demonstrates several important concepts:

1. **Modern Language Features**: Type safety, memory safety, expressive syntax
2. **Zero-cost Abstractions**: High-level code with embedded performance  
3. **Interoperability**: Seamless integration with existing C ecosystems
4. **Developer Experience**: Better tooling, debugging, and maintainability
5. **Future-proofing**: Modern language evolution for embedded development

The combination of Swift's safety and expressiveness with ESP-IDF's mature ecosystem opens new possibilities for embedded development.

## Next Step

Congratulations! You've successfully built and deployed a Swift Embedded application on ESP32-C6. You now have both a C-based MQTT gateway and a Swift-based IoT client running.

Next, we'll connect these two components together and demonstrate full device-to-device communication.

[Assignment 4: Device-to-Device Communication](../assignment-4)