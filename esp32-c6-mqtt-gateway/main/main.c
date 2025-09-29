#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "mqtt_gateway.h"

static const char *TAG = "MAIN";

// Task for periodic status monitoring
static void status_monitor_task(void *pvParameters)
{
    gateway_status_t status;
    
    while (1) {
        if (gateway_get_status(&status) == ESP_OK) {
            ESP_LOGI(TAG, "=== Gateway Status ===");
            ESP_LOGI(TAG, "WiFi Connected: %s", status.wifi_connected ? "Yes" : "No");
            ESP_LOGI(TAG, "MQTT Running: %s", status.mqtt_running ? "Yes" : "No");
            ESP_LOGI(TAG, "mDNS Running: %s", status.mdns_running ? "Yes" : "No");
            ESP_LOGI(TAG, "IP Address: %s", status.ip_address);
            ESP_LOGI(TAG, "MQTT Clients: %d", status.mqtt_client_count);
            ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
            ESP_LOGI(TAG, "====================");
        }
        
        vTaskDelay(pdMS_TO_TICKS(30000)); // Log status every 30 seconds
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP32-C6 MQTT Gateway starting...");
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
    
    // Initialize WiFi Manager
    ESP_LOGI(TAG, "Initializing WiFi Manager...");
    esp_err_t ret = wifi_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi Manager: %s", esp_err_to_name(ret));
        return;
    }
    
    // Start WiFi connection
    ESP_LOGI(TAG, "Starting WiFi connection...");
    ret = wifi_manager_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return;
    }
    
    // Wait for WiFi connection
    int retry_count = 0;
    while (!wifi_manager_is_connected() && retry_count < 20) {
        ESP_LOGI(TAG, "Waiting for WiFi connection... (%d/20)", retry_count + 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        retry_count++;
    }
    
    if (!wifi_manager_is_connected()) {
        ESP_LOGE(TAG, "Failed to connect to WiFi after 20 seconds");
        return;
    }
    
    ESP_LOGI(TAG, "WiFi connected successfully!");
    
    // Initialize mDNS service
    ESP_LOGI(TAG, "Initializing mDNS service...");
    ret = mdns_service_init();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to initialize mDNS: %s", esp_err_to_name(ret));
        // Continue without mDNS - it's not critical
    }
    
    // Initialize MQTT Gateway
    ESP_LOGI(TAG, "Initializing MQTT Gateway...");
    ret = mqtt_gateway_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MQTT Gateway: %s", esp_err_to_name(ret));
        return;
    }
    
    // Start MQTT Gateway
    ESP_LOGI(TAG, "Starting MQTT Gateway...");
    ret = mqtt_gateway_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT Gateway: %s", esp_err_to_name(ret));
        return;
    }
    
    ESP_LOGI(TAG, "ESP32-C6 MQTT Gateway initialized successfully!");
    ESP_LOGI(TAG, "Services running:");
    ESP_LOGI(TAG, "  - WiFi Station Mode");
    ESP_LOGI(TAG, "  - MQTT Client/Gateway");
    ESP_LOGI(TAG, "  - mDNS Service Discovery");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Gateway will bridge MQTT messages and provide status information.");
    ESP_LOGI(TAG, "Subscribe to '/gateway/status' to get gateway status.");
    ESP_LOGI(TAG, "Publish to '/test/topic' to test message routing.");
    
    // Create status monitoring task
    xTaskCreate(status_monitor_task, "status_monitor", 4096, NULL, 5, NULL);
    
    // Main loop - keep the application running
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Check if WiFi connection is lost and attempt to reconnect
        if (!wifi_manager_is_connected()) {
            ESP_LOGW(TAG, "WiFi connection lost, attempting to reconnect...");
            wifi_manager_start(); // This will retry the connection
        }
    }
}