#include "mqtt_gateway.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "mdns.h"
#include <string.h>

static const char *TAG = "MQTT_GATEWAY";

// MQTT client handle
static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;
static bool mqtt_running = false;
static int mqtt_client_count = 0;

// mDNS variables
static bool mdns_initialized = false;

// MQTT event handler
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mqtt_connected = true;
        
        // Subscribe to a test topic to demonstrate gateway functionality
        int msg_id = esp_mqtt_client_subscribe(client, "/test/topic", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        
        // Subscribe to gateway control topics
        msg_id = esp_mqtt_client_subscribe(client, "/gateway/status", 0);
        ESP_LOGI(TAG, "subscribed to gateway status topic, msg_id=%d", msg_id);
        
        break;
        
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_connected = false;
        break;
        
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
        
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
        
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
        
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        
        // Handle gateway status request
        if (strncmp(event->topic, "/gateway/status", event->topic_len) == 0) {
            // Publish gateway status
            gateway_status_t status;
            if (gateway_get_status(&status) == ESP_OK) {
                char status_msg[256];
                snprintf(status_msg, sizeof(status_msg), 
                        "{\"wifi_connected\":%s,\"mqtt_running\":%s,\"ip\":\"%s\",\"clients\":%d}",
                        status.wifi_connected ? "true" : "false",
                        status.mqtt_running ? "true" : "false",
                        status.ip_address,
                        status.mqtt_client_count);
                esp_mqtt_client_publish(client, "/gateway/status/response", status_msg, 0, 0, 0);
            }
        }
        break;
        
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
        
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_err_t mqtt_gateway_init(void)
{
    ESP_LOGI(TAG, "Initializing MQTT Gateway");
    
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = "mqtt://localhost:1883",  // This will be the local broker
        },
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }
    
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    
    ESP_LOGI(TAG, "MQTT Gateway initialized");
    return ESP_OK;
}

esp_err_t mqtt_gateway_start(void)
{
    ESP_LOGI(TAG, "Starting MQTT Gateway");
    
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_FAIL;
    }
    
    esp_err_t ret = esp_mqtt_client_start(mqtt_client);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client");
        return ret;
    }
    
    mqtt_running = true;
    ESP_LOGI(TAG, "MQTT Gateway started");
    return ESP_OK;
}

esp_err_t mqtt_gateway_stop(void)
{
    ESP_LOGI(TAG, "Stopping MQTT Gateway");
    
    if (mqtt_client != NULL) {
        esp_mqtt_client_stop(mqtt_client);
        esp_mqtt_client_destroy(mqtt_client);
        mqtt_client = NULL;
    }
    
    mqtt_running = false;
    mqtt_connected = false;
    ESP_LOGI(TAG, "MQTT Gateway stopped");
    return ESP_OK;
}

bool mqtt_gateway_is_running(void)
{
    return mqtt_running && mqtt_connected;
}

int mqtt_gateway_get_client_count(void)
{
    // For now, return a simulated count
    // In a real implementation, this would track actual client connections
    return mqtt_client_count;
}

esp_err_t mdns_service_init(void)
{
    ESP_LOGI(TAG, "Initializing mDNS service");
    
    esp_err_t err = mdns_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize mDNS: %s", esp_err_to_name(err));
        return err;
    }
    
    mdns_hostname_set("esp32c6-gateway");
    mdns_instance_name_set("ESP32-C6 MQTT Gateway");
    
    // Advertise MQTT service
    mdns_service_add("ESP32-C6-Gateway", "_mqtt", "_tcp", 1883, NULL, 0);
    
    // Advertise HTTP service if needed
    mdns_service_add("ESP32-C6-Gateway-Web", "_http", "_tcp", 80, NULL, 0);
    
    mdns_initialized = true;
    ESP_LOGI(TAG, "mDNS service initialized");
    return ESP_OK;
}

esp_err_t mdns_service_stop(void)
{
    if (mdns_initialized) {
        mdns_free();
        mdns_initialized = false;
        ESP_LOGI(TAG, "mDNS service stopped");
    }
    return ESP_OK;
}

esp_err_t gateway_get_status(gateway_status_t *status)
{
    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    memset(status, 0, sizeof(gateway_status_t));
    
    status->wifi_connected = wifi_manager_is_connected();
    status->mqtt_running = mqtt_gateway_is_running();
    status->mdns_running = mdns_initialized;
    status->mqtt_client_count = mqtt_gateway_get_client_count();
    
    // Get IP address
    if (status->wifi_connected) {
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        if (netif != NULL) {
            esp_netif_ip_info_t ip_info;
            if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                esp_ip4addr_ntoa(&ip_info.ip, status->ip_address, sizeof(status->ip_address));
            }
        }
    }
    
    return ESP_OK;
}