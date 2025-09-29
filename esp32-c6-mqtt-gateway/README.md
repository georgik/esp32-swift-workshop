# ESP32-C6 MQTT Gateway

This project implements an MQTT gateway using the ESP32-C6 microcontroller with the new `espressif/mqtt` component (version 1.0.0). It's designed for the Swift 6.2 workshop and provides MQTT client functionality with gateway capabilities.

## Features

- **WiFi Station Mode**: Connects to existing WiFi network
- **MQTT Client**: Uses the new espressif/mqtt component v1.0.0
- **mDNS Service Discovery**: Advertises the gateway on the local network
- **Gateway Status Monitoring**: Provides status information via MQTT
- **Automatic Reconnection**: Handles WiFi disconnections gracefully

## Hardware Requirements

- ESP32-C6 development board
- WiFi network access

## Software Dependencies

- ESP-IDF v5.0 or later
- espressif/mqtt component v1.0.0 (automatically managed via Component Manager)
- espressif/mdns component v1.8.2+ (automatically managed)

## Configuration

### WiFi Settings

Edit `sdkconfig.defaults` and update the WiFi credentials:

```
CONFIG_ESP_WIFI_SSID="YourWiFiSSID"
CONFIG_ESP_WIFI_PASSWORD="YourWiFiPassword"
```

Or use `idf.py menuconfig` to configure WiFi settings interactively.

## Building and Flashing

1. Make sure ESP-IDF is properly installed and sourced
2. Navigate to the project directory:
   ```bash
   cd /Users/georgik/projects/esp32-swift-workshop/esp32-c6-mqtt-gateway
   ```

3. Configure the project (if needed):
   ```bash
   idf.py menuconfig
   ```

4. Build the project:
   ```bash
   idf.py build
   ```

5. Flash to ESP32-C6:
   ```bash
   idf.py -p /dev/cu.usbserial-* flash monitor
   ```

## Usage

### Gateway Operation

Once running, the gateway will:

1. Connect to the configured WiFi network
2. Start the MQTT client service
3. Initialize mDNS for service discovery
4. Begin monitoring and logging status every 30 seconds

### MQTT Topics

The gateway subscribes to and responds to the following topics:

- `/gateway/status` - Request gateway status
- `/gateway/status/response` - Gateway status response (JSON format)
- `/test/topic` - Test topic for message routing

### Status Information

Gateway status is published in JSON format:
```json
{
  "wifi_connected": true,
  "mqtt_running": true,
  "ip": "192.168.1.100",
  "clients": 0
}
```

### mDNS Services

The gateway advertises the following mDNS services:

- `_mqtt._tcp` on port 1883
- `_http._tcp` on port 80 (for future web interface)

## Architecture

The project is structured as follows:

- **main.c**: Main application logic and initialization
- **wifi_manager.c**: WiFi connection and management
- **mqtt_gateway.c**: MQTT client functionality and message handling
- **mqtt_gateway.h**: Header file with function declarations and structures

## Key Differences from C3 Version

This C6 implementation differs from the original iotcraft-gateway (C3) in several ways:

1. **MQTT Component**: Uses the new `espressif/mqtt` v1.0.0 instead of Mosquitto broker
2. **No NAT**: Simplified network configuration without NAT functionality
3. **Client Mode**: Operates as MQTT client rather than broker
4. **Reduced Complexity**: Focused on gateway functionality without DHCP server

## Troubleshooting

### Common Issues

1. **WiFi Connection Failed**: 
   - Check WiFi credentials in `sdkconfig.defaults`
   - Ensure the ESP32-C6 is within WiFi range
   - Verify the network supports WPA2/WPA3

2. **MQTT Connection Issues**:
   - The current implementation tries to connect to `localhost:1883`
   - Modify the MQTT broker URI in `mqtt_gateway.c` to point to your MQTT broker

3. **Build Errors**:
   - Ensure ESP-IDF v5.0+ is installed
   - Run `idf.py clean` and rebuild if needed

### Monitoring

Use the serial monitor to view logs:
```bash
idf.py monitor
```

The gateway will log status information every 30 seconds, including:
- WiFi connection status
- MQTT connection status  
- IP address
- Free heap memory

## Future Enhancements

Potential improvements for this gateway:

- HTTP web interface for configuration
- MQTT broker mode (using Mosquitto component)
- Support for multiple MQTT brokers
- Configuration via web interface or BLE
- Data logging and analytics
- Security enhancements (TLS/SSL)

## License

This project follows the same licensing as ESP-IDF components (Apache License 2.0).