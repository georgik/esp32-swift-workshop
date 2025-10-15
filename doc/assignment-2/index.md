---
title: "ESP32-C6 Swift Embedded Workshop - Assignment 2: MQTT Gateway (C Implementation)"
date: 2024-10-15T00:00:00+01:00
showTableOfContents: false
series: ["WS002"]
series_order: 3
showAuthor: false
---

## Assignment 2: MQTT Gateway (C Implementation)

---

In this assignment, you'll build and deploy a C-based MQTT gateway that provides the network infrastructure for our IoT ecosystem. This gateway will connect to WiFi, establish MQTT client functionality, and provide service discovery through mDNS.

### Learning Objectives

By the end of this assignment, you will:

- Understand ESP-IDF project structure and build system
- Configure WiFi credentials and network settings
- Implement MQTT client functionality using ESP-IDF components
- Set up mDNS service discovery for device networking
- Monitor and debug embedded applications using serial output

### Assignment Overview

The MQTT Gateway serves as the backbone of our IoT system:

- **WiFi Station Mode**: Connects to existing WiFi network
- **MQTT Client**: Communicates with external MQTT brokers
- **Message Routing**: Bridges between local devices and remote services
- **Service Discovery**: Advertises services via mDNS
- **Status Monitoring**: Provides system health and connectivity information

### Step 1: Examine Project Structure

Navigate to the gateway project and understand its architecture:

```bash
cd $ESP_WORKSHOP_PATH/esp32-c6-mqtt-gateway

# Examine the project structure
tree .
```

**Project Structure:**
```
esp32-c6-mqtt-gateway/
├── CMakeLists.txt              # Project configuration
├── partitions.csv              # Flash partition table
├── sdkconfig.defaults          # ESP-IDF default configuration
└── main/
    ├── CMakeLists.txt          # Main component configuration
    ├── idf_component.yml       # Component dependencies
    ├── main.c                  # Application entry point
    ├── mqtt_gateway.c          # MQTT client and gateway logic
    ├── mqtt_gateway.h          # Function declarations and types
    └── wifi_manager.c          # WiFi connection management
```

### Step 2: Understand the Code Architecture

#### main.c - Application Entry Point

```c path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-mqtt-gateway/main/main.c start=33
void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-C6 MQTT Gateway starting...");
    
    // Initialize WiFi Manager
    wifi_manager_init();
    wifi_manager_start();
    
    // Initialize MQTT Gateway  
    mqtt_gateway_init();
    mqtt_gateway_start();
    
    // Start status monitoring
    xTaskCreate(status_monitor_task, "status_monitor", 4096, NULL, 5, NULL);
}
```

#### wifi_manager.c - WiFi Connection Management

Key features:
- **Automatic Connection**: Connects to configured WiFi network
- **Retry Logic**: Attempts reconnection on failure
- **Event Handling**: Manages WiFi state changes
- **Status Tracking**: Reports connection status

#### mqtt_gateway.c - MQTT Client Implementation

Key features:
- **MQTT Client**: Uses espressif/mqtt v1.0.0 component
- **Topic Subscription**: Listens for gateway control messages
- **Message Publishing**: Responds to status requests
- **Connection Management**: Handles MQTT broker connectivity

#### mDNS Service Discovery

Advertises services on the local network:
- **_mqtt._tcp**: MQTT service on port 1883
- **_http._tcp**: HTTP service on port 80 (for future web interface)

### Step 3: Configure WiFi Credentials

Edit the WiFi configuration for your network:

```bash
# Option 1: Edit sdkconfig.defaults directly
nano sdkconfig.defaults

# Change these lines to match your WiFi network:
CONFIG_ESP_WIFI_SSID="YourWiFiSSID"
CONFIG_ESP_WIFI_PASSWORD="YourWiFiPassword"
```

**Alternative Configuration (Interactive):**
```bash
# Option 2: Use interactive configuration
idf.py menuconfig

# Navigate to: Component config > ESP Wi-Fi > WiFi SSID/Password
```

### Step 4: Examine Component Dependencies

Check the component dependencies in `main/idf_component.yml`:

```yaml path=/Volumes/ssdt5/Users/georgik/projects/esp32-swift-workshop/esp32-c6-mqtt-gateway/main/idf_component.yml start=1
dependencies:
  espressif/mqtt: "^1.0.0"
  espressif/mdns: ">=1.8.2,<2.0.0"
```

These components provide:
- **espressif/mqtt**: Modern MQTT client implementation
- **espressif/mdns**: Service discovery functionality

### Step 5: Build the Gateway Project

Configure and build the project:

```bash
# Set the ESP32-C6 target
idf.py set-target esp32c6

# Build the project
idf.py build
```

**Expected Build Output:**
```
Project build complete. To flash, run:
  idf.py -p /dev/cu.usbmodem* flash monitor

Binary size: ~500KB
```

**Build Troubleshooting:**

If you encounter build errors:

```bash
# Clean and rebuild
idf.py fullclean
idf.py build

# Check component versions
idf.py dependencies
```

### Step 6: Flash and Monitor the Gateway

Deploy the firmware to your ESP32-C6:

```bash
# Flash and start monitoring (replace with your port)
idf.py -p /dev/cu.usbmodem* flash monitor
```

**Expected Serial Output:**
```
I (123) MAIN: ESP32-C6 MQTT Gateway starting...
I (134) WIFI_MANAGER: Initializing WiFi Manager
I (145) WIFI_MANAGER: WiFi Manager initialized
I (156) WIFI_MANAGER: Starting WiFi connection
I (1234) WIFI_MANAGER: connected to ap SSID:YourWiFiSSID
I (1245) WIFI_MANAGER: got ip:192.168.1.100
I (1256) MQTT_GATEWAY: Initializing MQTT Gateway
I (1267) MQTT_GATEWAY: MQTT Gateway initialized
I (1278) MQTT_GATEWAY: Starting MQTT Gateway
I (2345) MQTT_GATEWAY: MQTT_EVENT_CONNECTED
I (2356) MQTT_GATEWAY: sent subscribe successful, msg_id=12345
I (2367) MAIN: ESP32-C6 MQTT Gateway initialized successfully!
```

Press `Ctrl+]` to exit the monitor.

### Step 7: Test Gateway Functionality with Physical Hardware

#### Test 1: WiFi Connection

Verify WiFi connectivity by checking the serial output for:
- WiFi initialization success
- IP address assignment
- Connection stability

#### Test 2: MQTT Broker Connection

**Note:** The current configuration attempts to connect to `localhost:1883`. For testing, you'll need to:

1. **Set up a local MQTT broker** or
2. **Modify the broker configuration**

**Option 1: Install Local MQTT Broker**
```bash
# macOS
brew install mosquitto
mosquitto -v

# Linux
sudo apt-get install mosquitto
sudo systemctl start mosquitto
```

**Option 2: Use Public Test Broker**

Edit `mqtt_gateway.c` to use a test broker:
```c
const esp_mqtt_client_config_t mqtt_cfg = {
    .broker = {
        .address.uri = "mqtt://test.mosquitto.org:1883",
    },
};
```

#### Test 3: mDNS Service Discovery

From another device on the same network:

```bash
# macOS/Linux - discover mDNS services
dns-sd -B _mqtt._tcp

# Should show: esp32c6-gateway._mqtt._tcp
```

#### Test 4: Gateway Status Monitoring

The gateway publishes status every 30 seconds. Monitor the serial output for:

```
I (30000) MAIN: === Gateway Status ===
I (30001) MAIN: WiFi Connected: Yes
I (30002) MAIN: MQTT Running: Yes
I (30003) MAIN: mDNS Running: Yes
I (30004) MAIN: IP Address: 192.168.1.100
I (30005) MAIN: MQTT Clients: 0
I (30006) MAIN: Free heap: 250000 bytes
```

### Step 7A: Alternative - Test with Wokwi Simulation

If you don't have ESP32-C6 hardware available, you can test the gateway using Wokwi simulation:

#### Build for Wokwi Simulation

```bash
# Build the project first
idf.py build

# The wokwi.toml file is already configured in the project
cat wokwi.toml
```

**Wokwi Configuration** (`wokwi.toml`):
```toml
[wokwi]
version = 1
firmware = 'build/esp32-c6-mqtt-gateway.bin'
elf = 'build/esp32-c6-mqtt-gateway.elf'

[[wokwi.parts]]
type = 'wokwi-esp32-c6'
id = 'esp'

[[wokwi.parts]]  
type = 'wokwi-led'
id = 'led1'
attrs = { color = 'red' }

[[wokwi.parts]]
type = 'wokwi-resistor' 
id = 'r1'
attrs = { value = '220' }

# LED connected to GPIO 8 for status indication
[[wokwi.connections]]
from = 'esp:8'
to = 'r1:1'

[[wokwi.connections]]
from = 'r1:2'
to = 'led1:A'

[[wokwi.connections]]
from = 'led1:C'
to = 'esp:GND.1'
```

#### Run Wokwi Simulation

**Option 1: Using Wokwi CLI**
```bash
# Start Wokwi simulation
wokwi --timeout 30000

# The simulation will:
# 1. Load the compiled firmware
# 2. Start the ESP32-C6 simulation
# 3. Connect to simulated WiFi (Wokwi-GUEST)
# 4. Show serial output in terminal
```

**Option 2: Using Wokwi Online**
1. Go to [wokwi.com](https://wokwi.com)
2. Create new ESP32-C6 project
3. Upload your `build/esp32-c6-mqtt-gateway.bin` file
4. Configure the circuit with LED on GPIO 8
5. Start simulation

**Option 3: Using VS Code Extension**
1. Open project in VS Code
2. Press `F1` and type "Wokwi: Start Simulation"
3. The extension will automatically use `wokwi.toml` configuration

#### Simulated Testing Benefits

- **No Hardware Required**: Test without ESP32-C6 board
- **Visual Feedback**: See LED status changes in real-time
- **Debug Access**: Full debugging capabilities with breakpoints
- **Network Simulation**: Wokwi provides simulated internet connectivity
- **Fast Iteration**: Immediate feedback without flashing hardware

#### Simulation Limitations

- **MQTT Broker**: Limited external network access in simulation
- **Real-time Constraints**: Simulation may not match exact hardware timing
- **Hardware-specific Features**: Some ESP32-C6 features may not be fully simulated
- **Performance**: Simulation runs slower than real hardware

### Step 8: Test MQTT Communication

If you have an MQTT broker running, test the gateway's MQTT functionality:

```bash
# Subscribe to gateway status responses
mosquitto_sub -h localhost -t "/gateway/status/response"

# Request gateway status
mosquitto_pub -h localhost -t "/gateway/status" -m "request"

# Expected response:
# {"wifi_connected":true,"mqtt_running":true,"ip":"192.168.1.100","clients":0}
```

### Step 9: Advanced Configuration

#### Customize MQTT Broker Address

For production use, configure the MQTT broker address:

1. **Edit `mqtt_gateway.c`:**
   ```c
   const esp_mqtt_client_config_t mqtt_cfg = {
       .broker = {
           .address.uri = "mqtt://your-broker-ip:1883",
       },
   };
   ```

2. **Rebuild and reflash:**
   ```bash
   idf.py build
   idf.py -p /dev/cu.usbmodem* flash monitor
   ```

#### Adjust Memory Settings

For applications requiring more sockets:

1. **Check `CMakeLists.txt`:**
   ```cmake
   # Define FD_SETSIZE to support more LWIP sockets
   add_compile_definitions(FD_SETSIZE=64)
   ```

2. **Verify `sdkconfig.defaults`:**
   ```
   CONFIG_LWIP_MAX_SOCKETS=64
   CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192
   ```

### Step 10: Understanding the Component System

The gateway uses ESP-IDF's component manager:

```bash
# Check component dependencies
idf.py dependencies

# Update components to latest versions
idf.py update-dependencies
```

**Key Components:**
- **espressif/mqtt**: Provides `esp_mqtt_client.h` and MQTT functionality
- **espressif/mdns**: Provides `mdns.h` and service discovery
- **Built-in components**: WiFi, TCP/IP stack, FreeRTOS

### Troubleshooting Common Issues

#### WiFi Connection Issues

**Problem**: WiFi fails to connect
```bash
# Check WiFi credentials in sdkconfig.defaults
grep CONFIG_ESP_WIFI_ sdkconfig.defaults

# Verify network supports WPA2/WPA3
# Check if 2.4GHz network is available (ESP32-C6 doesn't support 5GHz)
```

**Problem**: WiFi connects but gets no IP
```bash
# Check DHCP settings on router
# Verify network allows new device connections
# Monitor serial output for IP assignment messages
```

#### MQTT Connection Issues

**Problem**: MQTT fails to connect
```bash
# Verify MQTT broker is running
mosquitto -v  # Should show broker starting

# Check broker address in code
grep "localhost" main/mqtt_gateway.c

# Test broker connectivity from computer
mosquitto_pub -h localhost -t test -m "hello"
```

**Problem**: MQTT connects but no messages
```bash
# Check topic subscriptions in serial output
# Verify broker allows anonymous connections
# Test with mosquitto_pub/sub tools
```

#### Build Issues

**Problem**: Component not found
```bash
# Update component registry
idf.py update-dependencies

# Check internet connection for component download
# Verify idf_component.yml syntax
```

**Problem**: Flash size errors
```bash
# Check partition table
cat partitions.csv

# Verify flash size matches hardware
# Use idf.py size to check binary size
```

### Understanding the Gateway's Role

This gateway serves several important functions in our IoT ecosystem:

1. **Network Bridge**: Connects local devices to external MQTT brokers
2. **Service Discovery**: Advertises local services via mDNS
3. **Message Routing**: Forwards messages between devices and cloud services  
4. **Status Monitoring**: Provides health and connectivity information
5. **Connection Management**: Handles network reconnections gracefully

The gateway will be the foundation for connecting our Swift client in the next assignment.

## Next Step

Excellent work! You now have a functional MQTT gateway providing network infrastructure. The gateway is connected to WiFi, communicating with MQTT brokers, and advertising its services.

Next, we'll create the Swift Embedded client that will communicate with this gateway.

[Assignment 3: Swift Client Development](../assignment-3)