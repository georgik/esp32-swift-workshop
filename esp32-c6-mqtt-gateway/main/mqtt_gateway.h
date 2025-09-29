#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

// WiFi Manager functions
esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_start(void);
esp_err_t wifi_manager_stop(void);
bool wifi_manager_is_connected(void);

// MQTT Gateway functions
esp_err_t mqtt_gateway_init(void);
esp_err_t mqtt_gateway_start(void);
esp_err_t mqtt_gateway_stop(void);
bool mqtt_gateway_is_running(void);
int mqtt_gateway_get_client_count(void);

// mDNS functions
esp_err_t mdns_service_init(void);
esp_err_t mdns_service_stop(void);

// Gateway status structure
typedef struct {
    bool wifi_connected;
    bool mqtt_running;
    bool mdns_running;
    int mqtt_client_count;
    char ip_address[16];
} gateway_status_t;

esp_err_t gateway_get_status(gateway_status_t *status);

#ifdef __cplusplus
}
#endif